--必须在这个位置定义PROJECT和VERSION变量
--PROJECT：ascii string类型，可以随便定义，只要不使用,就行
--VERSION：ascii string类型，如果使用Luat物联云平台固件升级的功能，必须按照"X.X.X"定义，X表示1位数字；否则可随便定义
PROJECT = "FTP"
VERSION = "2.0.0"
--加载日志功能模块，并且设置日志输出等级
--如果关闭调用log模块接口输出的日志，等级设置为log.LOG_SILENT即可

require "net"
require "netLed"
require "wdt"
require "websocket"
require "socket"
require "io"
require "misc"
require "ntp"
require "utils"
require "socket4G"
require "ftp"
require "clib"
require "sys"
require "pins"
-- require "basic"
ril.request("AT+RNDISCALL=0,1")
-- 开启NTP 授时
ril.regUrc("+NITZ", function()    
    bTimeSyned = false--打开NTP授时
end)

sys.subscribe("IP_READY_IND", function()
    if not bTimeSyned then    
        ntp.timeSync(nil,function(tClock,success)
            bTimeSyned = success
        end)
    end
end)
-- local ip, port = "103.46.128.49", "17353"
local ip, port = "124.221.214.114", "8083"
local msgQuene = {}

local function insertMsg(data)
    table.insert(msgQuene,data)
end

function waitForSend()
    return #msgQuene > 0
end

local uploadFlag = 0
local powerfailure = 0
local  fileName;
-- Sd卡挂载
local sdCardFlag = io.mount(io.SDCARD)
--串口初始化
uart.setup(1,115200,8,uart.PAR_NONE,uart.STOP_1,0,0,0) 
-- 掉电检测 中断关闭文件系统
pins.setup(pio.P0_14, function()
    log.info("gpio", "中断关闭文件系统")
    sdCardFlag = 0
    powerfailure = 1
    if fd then 
        io.unmount(io.SDCARD)
    end
end, pio.PULLDOWN
)
-- FTP 回传任务
pins.setup(pio.P0_15, function()
    log.info("gpio", "开启FTP回传任务")
        -- 校验和测试  
        --    这里进行回传任务 数据就不Push到队列里边直接进行回传任务
            sys.taskInit(function()
            -- 开启回传任务
            uploadFlag  = 1
            local r, n = ftp.login("PASV", "144.123.30.226", 21, "feilog", "ydhy@160725") -- 登录
            if r == "426" or r == "503" then return end
            ftp.cwd("/flylog")
            local TClock = os.date("*t")--查询模块系统时间
            local Time= string.format("1_%04d_%02d_%02d",TClock.year,TClock.month,TClock.day)
            -- 这里根据配置创建文件夹
            local YearDirPath  = TClock.year
            -- 先模拟
            local DriverID = "1"
            -- 年文件夹
            ftp.mkd("/flylog".."/"..YearDirPath)
            ftp.cwd("/flylog".."/"..YearDirPath)
            -- 设备文件夹
            ftp.mkd("/flylog".."/"..YearDirPath.."/"..DriverID)
            ftp.cwd("/flylog".."/"..YearDirPath.."/"..DriverID)
            local FileName =  "/".. Time .. ".txt"
            fileName =  "/sdcard0".."/"..Time .. ".txt"
            res = io.open(fileName,"a+")
            local targetFile  = "/flylog".."/"..YearDirPath.."/"..DriverID..FileName
            ftp.checktype("I")
            local t = ftp.upload(targetFile,fileName) -- 从sd卡目录上传文件至服务器
            -- 上传完毕后查询上传文件大小跟本地做比较如果不同
            ftp.cwd("/flylog".."/"..YearDirPath.."/"..DriverID)
            ftp.close()
            -- 再次连接获取服务器上传文件 
            ftp.login("PASV", "144.123.30.226", 21, "feilog", "ydhy@160725") -- 登录
            ftp.checktype("I")
            r, data = ftp.list(targetFile)
            res = (string.len(FileName)-1)
            res =  string.sub(data,-res)
            -- 简单的正则表达式
            res = string.gsub(res, "[ \t\n\r]+$", "")
            -- 从后向前截取子串获取文件名 如果相同保证上传文件//
            ftpfileName = '/'..DriverID ..'_'..res
            print(ftpfileName)
            print(FileName)
            print(string.len(ftpfileName))
            print(string.len(FileName))
            -- -- 名字相同
            if ftpfileName == FileName then
              local FileSize = string.sub(data,-((string.len(ftpfileName)+2)+18),-(string.len(ftpfileName)+2))
              ftpFileSize = string.gsub(FileSize, "%s+", "")
              localFile  = io.fileSize(fileName)
              if ftpFileSize ==  localFile then
                -- 如果
                uploadFlag  = 0
                log.info("DEBUG","UPLOAD END")
              end
            end
                ftp.close()
                res:close()
            end)
end, pio.PULLDOWN
)



-- 处理队列里数据任务
function outMsgprocess(socketClient)
    --队列中有消息
    while #msgQuene>0 do
        --获取消息，并从队列中删除
        local outMsg = table.remove(msgQuene,1)
        -- 如果sd卡挂在 且并没有任务ftp回传则记录文件至sd卡
        if sdCardFlag == 1 and uploadFlag  == 0 then
        local result = socketClient:asyncSend(outMsg)
        -- 先获取当日时间  根据时间  是否创建文件
        local tClock = os.date("*t")--查询模块系统时间
        local time = string.format("1_%04d_%02d_%02d",tClock.year,tClock.month,tClock.day)
        fileName =  "/sdcard0".."/"..time .. ".txt"
            local fd = io.open(fileName,"a+")
            if fd then
                -- 添加换行
                fd:write(outMsg.."\n", "a+")
                fd:close()
            end
        --发送失败的话立刻返回nil（等同于false）
        if not result then return end
        end
    end
    return true
end


--  串口回调事件
local function uart1ReceiveCb()
    local data=""
    local temp=""
    while true do
        temp = uart.read(1,100)  --每次尝试读100字节
        if not temp or string.len(temp) == 0 then   --如果把接收缓冲区读空了
            break
        else
            data = data..temp    --拼接
        end
        --
        header1 =  string.byte(data, 1)
        header2 =  string.byte(data, 2)
        if  header1 == 0xFB and  header2 == 0x90 then
            -- 这里是实时后台任务
            insertMsg(data)
        end
    end
end

-- 串口一事件回调
uart.on(1,"receive",uart1ReceiveCb)

sys.init(0, 0)

local SocketClient
-- 创建连接//
sys.taskInit(function()
    while true do
        while not socket.isReady() do sys.wait(1000) end
        SocketClient = socket.tcp()
        while not SocketClient:connect(ip, port) do sys.wait(2000) end
        while SocketClient:asyncSelect() do 
        end
        SocketClient:close()
    end
end)

sys.taskInit(function()

    while true do
    local SdCardState
    local Net4GState
    local UploadState
    local PowerState
    if sdCardFlag  ~=  1 then
        -- SD 卡状态  
        SdCardState  = 0x01
    else
        SdCardState  = 0x00
    end
    --   FTP下载状态
    if uploadFlag ~= 1 then 
        UploadState   = 0x00
    else
        UploadState   = 0x01
    end
    -- 模块网络状态
    -- 发送错误报文给stm32 然后拆卸重新挂载
    if not socket.isReady()then
        Net4GState  = 0x01
    else
        Net4GState  = 0x00
    end

    if powerfailure ~=1 then
        PowerState = 0x00
    else
        PowerState = 0x01
    end

    -- 发送当前状态
    uart.write(1,0xFB,0x90,SdCardState,Net4GState,UploadState,PowerState,0x00)
    sys.wait(2000)
    end
end)

-- 信息发送
sys.taskInit(function()
    while not socket.isReady() do sys.wait(2000) end
    sys.wait(1000)
    while true do
    outMsgprocess(SocketClient)
    sys.wait(1000)
    end
end)

sys.run()


