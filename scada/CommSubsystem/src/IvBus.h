/***********************************************************
*
*
*  IvBus.h
*
*  Create by muxd
*
*  2017/09/26
***********************************************************/
#ifndef __I_VBUS_H__
#define __I_VBUS_H__

//
// brief vBusģ�����ӿ�
//
//       ʵ����Դ������������
class IvBus
{
public:
    virtual ~IvBus() {};

    typedef enum interactive_type
    {
        REALTIME,  //��ʱ����
        PERIODIC,  //�ȴ�����ʱ϶������
    } INTERACTIVE_TYPE;

    typedef enum host_state
    {
        HOST_STATE_ERROR,
        HOST_STATE_OFFLINE,
        HOST_STATE_ONLINE,
    } HOST_STATE;

    //
    //! @brief ��ȡ����Ȩ��
    //���ȴ�����ʱ϶��ͨ��ִ�о���Ĳ���֮ǰҪ���ô������ӿڣ�
    //ȷ���Ѿ���ȡ��ʱ϶��ִ�к����Ĳ���
    //
    // @param
    //       comType: Ȩ������
    //       iApp:    Ӧ��ID
    //
    // @return value
    //       0: Success
    //      -1: TimeOut
    virtual int getInteractiveAuth(
        INTERACTIVE_TYPE comType ) = 0;

    //
    //! @brief �ͷŽ���Ȩ��
    //���ȴ�����ʱ϶��ͨ��ִ�о���Ĳ���֮ǰҪ���ô������ӿڣ�
    //ȷ���Ѿ���ȡ��ʱ϶��ִ�к����Ĳ���
    //
    // @param
    //       comType: Ȩ������
    //       iApp:    Ӧ��ID
    //
    // return value
    //       0: Success
    //      <0: Error
    virtual int releaseInteractiveAuth(
        INTERACTIVE_TYPE comType ) = 0;


    //
    //!@brief ��ȡĳһ�����ڵ��״̬
    //
    // @param
    //       hostId: �����ڵ�ID
    //
    // return �����ڵ�Ĺ���״̬
    //        HOST_STATE
    virtual HOST_STATE getHostState( int hostId ) = 0;

    //
    // brief �������Ĵ����ӿ�
    //       ����ģʽ
    static IvBus* Instance( int iApp, int hostId );

};

#endif

