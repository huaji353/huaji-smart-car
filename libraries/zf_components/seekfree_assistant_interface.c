#include "seekfree_assistant.h"




//-------------------------------------------------------------------------------------------------------------------
// �������     ������ַ��ͺ���
// ����˵��     *buff           ��Ҫ���͵����ݵ�ַ
// ����˵��     length          ��Ҫ���͵ĳ���
// ���ز���     uint32          ʣ��δ�������ݳ���
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
uint32 (*seekfree_assistant_transfer_callback)   (const uint8 *buff, uint32 length);


//-------------------------------------------------------------------------------------------------------------------
// �������     ������ֽ������ݺ���
// ����˵��     *buff           ��Ҫ���յ����ݵ�ַ
// ����˵��     length          Ҫ���յ�������󳤶�
// ���ز���     uint32          ���յ������ݳ���
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
uint32 (*seekfree_assistant_receive_callback)    (uint8 *buff, uint32 length);


//-------------------------------------------------------------------------------------------------------------------
// �������     ������ֽӿ� ��ʼ��
// ����˵��
// ���ز���     void
// ʹ��ʾ��     seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI); ʹ�ø���WIFI SPIģ����������շ�
// ��ע         ��Ҫ���е����豸�ĳ�ʼ��������ʹ������ת���ڽ������ݵ��շ�������Ҫ���е�������ת���ڵĳ�ʼ����Ȼ���ٵ���seekfree_assistant_interface_init���������ֵĽӿڳ�ʼ��
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
            // �����Լ������� ����ʵ��seekfree_assistant_transfer��seekfree_assistant_receive������������ݵ��շ�
            
        }break;
    }
}


