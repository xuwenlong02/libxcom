#include <dataitem.h>

int main(int argc, const char *argv[]) {
    setenv("LOG", "stdout", 1);
    ID_DEV devId("51H");
    const std::string templ = "#modbus协议\n" 
"def crc16: $crc($1,0x8005,0xffff,0x0000,0)\n"
"# $addr $len 都是输入的参数\n"
"def send:     [$addr 03H $1  $itom(2,$len) $crc16($group(0,$pos))]\n"
"def recv:     [$addr 03H     $ditch(1,$assign(&dl,$0))     $data($ditch($dl))  $ditch(2,$chk($0,$crc16($group(0,$pos))))]"; 
    DataItem ditem(devId, templ);
    ditem.SetRegLength(2); // 设置寄存器长度， 即$len 的值
    ditem.ParseDataExpr("$int($data)", 1); // 最终的结果进行计算，这里是对recv 挖取的$data 转换成浮点数，1.0 表示比例因子为1， 即将结果再除以1.0

    std::vector<std::string> params;
    params.push_back("04H");
    ditem.ParseParams(params); // 这里传入的是参数列表, 对应着$1, $2, $3 等等。这里只有一个参数
    
    // 生成发生命令
    OpValue cmd = ditem.GenerateCmd("send");
    cmd.Show();
    if (cmd.IsEmpty())
        return -1;
    CmdValue genCmd = cmd.GenCmd();
    genCmd.Show();

    // 处理接收指令
    OpValue result;
    uint8_t cmd_buf[] = {0x51,0x03,0x04,0x00,0x00,0x00,0x1E,0x2A,0x3E};
    CmdValue recv(cmd_buf, sizeof(cmd_buf)/sizeof(cmd_buf[0]));
    recv.Show();
    if (ditem.ParseRecvCmd(recv, "recv") != RECV_COMPLETE) {
        return -1;
    }
    result = ditem.Result();
    result.Show();
    if (result != 0x1E) {
        return -1;
    }
    return 0;
}