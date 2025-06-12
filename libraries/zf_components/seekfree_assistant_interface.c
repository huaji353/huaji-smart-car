#include "seekfree_assistant.h"




//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手发送函数
// 参数说明     *buff           需要发送的数据地址
// 参数说明     length          需要发送的长度
// 返回参数     uint32          剩余未发送数据长度
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
uint32 (*seekfree_assistant_transfer_callback)   (const uint8 *buff, uint32 length);


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手接收数据函数
// 参数说明     *buff           需要接收的数据地址
// 参数说明     length          要接收的数据最大长度
// 返回参数     uint32          接收到的数据长度
// 使用示例
//-------------------------------------------------------------------------------------------------------------------
uint32 (*seekfree_assistant_receive_callback)    (uint8 *buff, uint32 length);


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     逐飞助手接口 初始化
// 参数说明
// 返回参数     void
// 使用示例     seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI); 使用高速WIFI SPI模块进行数据收发
// 备注         需要自行调用设备的初始化，例如使用无线转串口进行数据的收发，则需要自行调用无线转串口的初始化，然后再调用seekfree_assistant_interface_init完成逐飞助手的接口初始化
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_interface_init (seekfree_assistant_transfer_device_enum transfer_device)
{
    switch(transfer_device)
    {
        case SEEKFREE_ASSISTANT_DEBUG_UART:
        {
            seekfree_assistant_transfer_callback = debug_send_buffer;
            seekfree_assistant_receive_callback = debug_read_buffer;
        }break;
        

        case SEEKFREE_ASSISTANT_BLE6A20:
        {
            seekfree_assistant_transfer_callback = ble6a20_send_buffer;
            seekfree_assistant_receive_callback = ble6a20_read_buffer;
        }break;

        case SEEKFREE_ASSISTANT_CUSTOM:
        {         
            // 根据自己的需求 自行实现seekfree_assistant_transfer与seekfree_assistant_receive函数，完成数据的收发
            
        }break;
    }
}


