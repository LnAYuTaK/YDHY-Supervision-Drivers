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
require "pins"
require "sys"
require "ril"
require "pio"
require "sim"
require "log"
require "string"
require "crypto"
require "common"
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

-- Global Config define
local ServerIP = ""
local ServerPort = ""
local DriverID = ""
local FtpIP = ""
local key = "1234567890123456"
-- Global State define 
local uploadFlag = 0
local powerfailure = 0
local ConfigSuccess = 0

-- Other define 
local  fileName
local fd

-- Only User Test 
local IP, Port = "103.46.128.49", "17353"
-- local ServerIP, ServerPort = "124.221.214.114", "8083"
local msgQuene = {}

local function insertMsg(data)
    table.insert(msgQuene,data)
end

function waitForSend()
    return #msgQuene > 0
end

-- FTP回传data解码 暂时光写了list的回传数据解码
-- Return Hex 
-- 30322D30342D3233
-- 2020
-- 30393A3535414D
-- 2020202020202020202020202020202020
-- 38353530
-- 20
-- 313233345F323032335F30325F30342E7478740D0A
-- Return Str
-- 02-04-23  09:52AM                 4800 1234_2023_02_04.txt

-- 根据返回data 获取服务器信息  暂时实现获取文件大小和文件名字
function decodeFTPData(recvData,cmd)
    if cmd == "filesize" then
    pre = "%d+ "..DriverID
    findfile  = string.sub(recvData, string.find(recvData, pre))
    if findfile == nil then return nil end
    targetfile = string.sub(findfile ,0,-(string.len(DriverID)+2))
    if  targetfile == nil then return nil end     
    return string.gsub(targetfile, "%s+", "")
    elseif cmd == "ftpfilename" then
    pre = "%d+_%d+_%d+_%d+.txt"
    targetfile = string.sub(recvData,string.find(recvData, pre))
    -- 没找到需要重新上传
    if targetfile == nil then  return nil
    else return string.gsub(targetfile, "%s+", "")
    end
    end
end

-- 向服务器查询是否存在文件保证文件不重复上传 
function isAllowUpload(filename,localfilename,LocalFILENAMET )
    r,data = ftp.login("PASV", "144.123.30.226", 21, "feilog", "ydhy@160725") -- 登录
    if r == "426" or r == "503" then  return end

-- 这里做文件夹检查
    local TClock = os.date("*t")--查询模块系统时间
    local YearDirPath  = TClock.year
    ftp.cwd("/flylog")
    -- 先模拟
    -- 年份文件夹
    ftp.mkd("/flylog".."/"..YearDirPath)
    ftp.cwd("/flylog".."/"..YearDirPath)
    -- 设备文件夹
    ftp.mkd("/flylog".."/"..YearDirPath.."/"..DriverID)
    ftp.cwd("/flylog".."/"..YearDirPath.."/"..DriverID)
    ftp.checktype("I")
    r, data = ftp.list(filename)
    if r == '503' or r == "426"then
        log.info("LIST",r)
        log.info("FTP","未找到当日FTP服务器文件"..filename.."准备上传")
        return 1
    end
         -- 名字相同说明已经上传  判断下FTP 服务器文件大小和本地文件大小和名称
    ftpfileName =  decodeFTPData(data,"ftpfilename")
    ftpFileSize = decodeFTPData(data,"filesize")
    -- 根据绝对路径获取本地文件大小
    localFileSize  = tostring(io.fileSize(LocalFILENAMET))
    log.info("FTP"," 本地当日文件大小 "..localFileSize)
    log.info("FTP"," FTP服务器当日文件大小 "..ftpFileSize)
    -- 如果找不到名字不同说明 近日没有上传直接上传就行
    log.info("FTP",ftpfileName.." "..localfilename)
    if ftpfileName == localfilename then
      if localFileSize == ftpFileSize then
        log.info("FTP","找到当日FTP服务器文件且大小相等 准备退出FTP服务")
        ftp.close()
        return 0
       else
        log.info("FTP","找到当日FTP服务器文件且不同 准备删除服务器文件后上传")
        ftp.deletefile(filename,20)
        ftp.close()
        return 1
       end
    else
        ftp.close()
        log.info("FTP","未找到当日FTP服务器文件 准备上传服务器文件")
        return 1
    end
end

-- 获取配置文件
function getAllConfig(file)
    local  rfile  = io.open(file,"r")
    for line in rfile:lines() do
        -- print(line)
        if (string.find(line, "^ServerIP=%d+.%d+.%d+.%d+")) then
            ServerIP = string.sub(line, string.find(line, "%d+.%d+.%d+.%d+"));
        end
        -- get Server Port
        if (string.find(line, "^ServerPort=%d+")) then
            ServerPort = string.sub(line, string.find(line, "%d+"));
        end
        -- get DriverID
        if (string.find(line, "^DriverID=%d+")) then
            DriverID = string.sub(line, string.find(line, "%d+"));
        end
        -- -- get FTP IP
        if (string.find(line, "^FtpIP=%d+.%d+.%d+.%d+")) then
            FtpIP = string.sub(line, string.find(line, "%d+.%d+.%d+.%d"))
        end
        -- return "success"
    end
    -- 有配置读取失败
    if  (ServerIP=="") or (ServerPort =="")or(DriverID =="")or(FtpIP=="") then
        return "error"
    end
    rfile:close()
end

-- Sd卡挂载
local sdCardFlag = io.mount(io.SDCARD)


--串口初始化
uart.setup(1,115200,8,uart.PAR_NONE,uart.STOP_1,0,0,0) 
-- GPIO中断 掉电检测关闭文件系统
uart.setup(2,115200,8,uart.PAR_NONE,uart.STOP_1,0,0,0) 

pins.setup(pio.P0_14, function()
    log.info("gpio", "中断关闭文件系统")
    sdCardFlag = 0
    powerfailure = 1
    if fd then
        io.unmount(io.SDCARD)
    end
end, pio.PULLUP
)

-- --  Test 测试版本
-- pins.setup(pio.P0_14, function()
--     log.info("gpio", "中断关闭文件系统")
--     sdCardFlag = 0
--     powerfailure = 1
--     if fd then
--         time = os.date("*t")--查询模块系统时间
--         log.info("PowerDown :",time)
--         io.unmount(io.SDCARD)
--         while true do
--             print("CallBack")
--         end
--     end
-- end, pio.PULLDOWN
-- )

--GPIO 中断 FTP 回传任务
pins.setup(pio.P0_15, function()
            sys.taskInit(function()
            log.info("gpio", "开启FTP回传任务")
            if (uploadFlag  ~= 1 and ConfigSuccess == 1)then
             -- 关闭文件系统
            uploadFlag  = 1
            local TClock = os.date("*t")--查询模块系统时间
            local Time= string.format(DriverID.."_%04d_%02d_%02d",TClock.year,TClock.month,TClock.day)
            local YearDirPath  = TClock.year
            -- 本地文件名字
            local FileName =  "/".. Time .. ".txt"
            -- 本地文件名字 不带路径
            local localFileName =  Time .. ".txt"
            -- 上传ftp文件绝对路径
            local targetFile  = "/flylog".."/"..YearDirPath.."/"..DriverID..FileName
            -- 本地文件绝对路径
            local LocalFILENAMET =  "/sdcard0".."/"..Time .. ".txt"
            -- 这里查询下是否能上传
            result = isAllowUpload(targetFile,localFileName,LocalFILENAMET)
            if (result == 1) then
            -- if(isAllowUpload(targetFile,localFileName,LocalFILENAMET) == 1) then
            sys.wait(100)
             -- 登录
            local r, n = ftp.login("PASV", "144.123.30.226", 21, "feilog", "ydhy@160725") -- 登录
            fileName =  "/sdcard0".."/"..Time .. ".txt"
            ftp.checktype("I")
              -- 从sd卡目录上传文件至服务器
            log.info("FTP", "开始FTP传输")
            ftp.upload(targetFile,fileName,20)
            log.info("FTP", "FTP传输结束")
            log.info("FTP", "本地上传文件名 "..fileName.."  FTP上传名字 "..targetFile)
            ftp.close()
            sys.wait(100)
            -- 上传完毕后查询上传文件大小跟本地做比较如果不同
            -- 这里获取下刚才上传的文件
            -- 再次连接获取服务器上传文件 
            log.info("FTP", "校验上传FTP文件")
            ftp.login("PASV", "144.123.30.226", 21, "feilog", "ydhy@160725") -- 登录
            ftp.checktype("I")
            r, data = ftp.list(targetFile,20)
            if r == '503'or r == "426"then
                log.info("FTP","未找到上传的FTP服务器文件"..targetFile.."需要重新点击上传")
                log.info("FTP",r)
                return 0
            end
            ftpfileName = decodeFTPData(data,"ftpfilename")
             -- -- 如果名字相同且大小相同则 切换正常状态灯
            if ftpfileName == localFileName then
               ftpFileSize = decodeFTPData(data,"filesize")
               localFileSize  = tostring(io.fileSize(LocalFILENAMET))
               log.info("FTP", string.len(ftpFileSize).."  "..string.len(localFileSize),ftpFileSize,localFileSize)
               if ftpFileSize ==  localFileSize then
                    log.info("FTP", "校验文件大小相同 准备退出FTP")
                    uploadFlag  = 0
               else
                    log.info("FTP", "校验文件大小不同 请重新上传")
               end
            else
                -- 未知原因导致上传文件名字不同
                log.info("FTP-Waring", ftpfileName)
                log.info("FTP-Waring", FileName)
            end
            ftp.close()
            end
            if(result == 0) then
                log.info("FTP", "校验文件大小相同 准备退出FTP!!!!")
                uploadFlag  = 0
            end
            end

            log.info("FTP", "FTP服务结束")
            end)
end, pio.PULLDOWN
)

-- 处理队列里数据任务
function outMsgprocess(socketClient)
    --队列中有消息
    while #msgQuene>0 and (socket.isReady()) and ConfigSuccess ==1 do
        --获取消息，并从队列中删除
        local outMsg = table.remove(msgQuene,1)
        -- 如果sd卡挂载且并没有ftp回传任务则记录文件至sd卡
        if sdCardFlag == 1 and uploadFlag  == 0 then
        local result = socketClient:asyncSend(outMsg)
        -- 先获取当日时间  根据时间  是否创建文件
        local tClock = os.date("*t")--查询模块系统时间
        local time = string.format(DriverID.."_%04d_%02d_%02d",tClock.year,tClock.month,tClock.day)
        fileName =  "/sdcard0".."/"..time .. ".txt"
        -- 加密模式：ECB, 填充方式：ZeroPadding, 密钥：1234567890123456, 密钥长度：128 bit
            fd = io.open(fileName,"a+")
            if fd then
                -- 添加换行 
                encodeStr = crypto.aes_encrypt("ECB", "ZERO", outMsg, key)
                fd:write(encodeStr.."\n")
                log.info("AES",string.toHex(encodeStr))
                log.info("SDCard",string.toHex(outMsg))
                fd:close()
            end
        --发送失败的话立刻返回nil（等同于false）
        if not result then return end
        end
    end
    return true
end

local function bin2hex(s)
    s=string.gsub(s,"(.)",function (x) return string.format("%02X ",string.byte(x)) end)
    return s
end

local h2b = {
    ["0"] = 0,
    ["1"] = 1,
    ["2"] = 2,
    ["3"] = 3,
    ["4"] = 4,
    ["5"] = 5,
    ["6"] = 6,
    ["7"] = 7,
    ["8"] = 8,
    ["9"] = 9,
    ["A"] = 10,
    ["a"] = 10,
    ["B"] = 11,
    ["b"] = 11,
    ["C"] = 12,
    ["c"] = 12,
    ["D"] = 13,
    ["d"] = 13,
    ["E"] = 14,
    ["e"] = 14,
    ["F"] = 15,
    ["f"] = 15
}

local function hex2bin( hexstr )
    local s = string.gsub(hexstr, "(.)(.)%s", function ( h, l )
         return string.char(h2b[h]*16+h2b[l])
    end)
    return s
end

-- 字符串插入
function string_insert(str,index,insertStr, flag) 
    if flag and string.find(str, flag) ~=nil then
        index = index + #flag
    end
    local pre = string.sub(str, 1, index -1)
    local tail = string.sub(str, index, -1)
    local createStr = string.format("%s%s%s", pre, insertStr, tail)
    return createStr
    end
--  串口回调事件
local function uart1ReceiveCb()
    -- if ConfigSuccess ==1 then
    local data=""
    local temp=""
    while true do
        temp = uart.read(1,100)  --每次尝试读100字节
        if not temp or string.len(temp) == 0 then   --如果把接收缓冲区读空了
            break
        else
            data = data..temp    --拼接
        end
        header1 =  string.byte(data, 1)
        header2 =  string.byte(data, 2)
        -- log.info("UART",string.toHex((temp)))
        -- 长度判断 49
        if(string.len(data) == 49)then
 
            -- 包头判断
            if  header1 == 0xFB and  header2 == 0x90 then
                -- 校验和判断
                -- 这里是实时后台任务
                local insertStr =  string_insert(string.format("%04x",DriverID),3,' ')
                data  = string.gsub(bin2hex(data),"00 01",insertStr,1)
                insertMsg(hex2bin(data))
            end
        end
    end
    end
-- end

-- 串口一事件回调
uart.on(1,"receive",uart1ReceiveCb)

sys.init(0, 0)
local SocketClient
-- 创建后台服务链接任务//
sys.taskInit(function()
    while true do
        while not socket.isReady() do sys.wait(1000) end
        SocketClient = socket.tcp()
        while not SocketClient:connect(ServerIP, ServerPort) do sys.wait(2000) end
        while SocketClient:asyncSelect() do 
        end
        SocketClient:close()
    end
end)

-- 读取文件 初始化配置
-- 注意！ 如果读取失败将显示 SD卡错误 其他任务不会开启
sys.taskInit(function()
    while true do
        if(getAllConfig("/sdcard0".."/".."Config"..".ini")=="error") then
            log.info("CONFIG","ERROR")
            sdCardFlag = 0
        else
        -- 读取配置初始化成功
        ConfigSuccess = 1
        return 0
        end
    end
end)

-- 回复stm32灯状态任务
sys.taskInit(function()
    while true do
    local SdCardState
    local Net4GState
    local UploadState
    local PowerState
    --  SD 卡状态
    if sdCardFlag  ~=  1 then
  
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
    if not socket.isReady()then
        Net4GState  = 0x01
    else
        Net4GState  = 0x00
    end
    -- 掉电状态
    if powerfailure ~=1 then
        PowerState = 0x00
    else
        PowerState = 0x01
    end
    -- 发送当前状态
    uart.write(1,0xFB,0x90,SdCardState,Net4GState,UploadState,PowerState,0x00)
    log.info("LED STATE",SdCardState,Net4GState,UploadState,PowerState)
    sys.wait(2000)
    end
end)

-- 信息发送任务
sys.taskInit(function()
    if ConfigSuccess == 1 then
    while not socket.isReady() do sys.wait(2000) end
    sys.wait(1000)
    while true do
    outMsgprocess(SocketClient)
    sys.wait(1000)
    end
    end
end)

sys.run()
