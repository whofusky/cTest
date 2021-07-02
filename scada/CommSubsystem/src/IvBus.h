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
// brief vBus模块对外接口
//
//       实现资源的周期性利用
class IvBus
{
public:
    virtual ~IvBus() {};

    typedef enum interactive_type
    {
        REALTIME,  //即时链接
        PERIODIC,  //等待分配时隙的链接
    } INTERACTIVE_TYPE;

    typedef enum host_state
    {
        HOST_STATE_ERROR,
        HOST_STATE_OFFLINE,
        HOST_STATE_ONLINE,
    } HOST_STATE;

    //
    //! @brief 获取交互权限
    //仅等待分配时隙的通信执行具体的操作之前要调用此阻塞接口，
    //确保已经获取到时隙区执行后续的操作
    //
    // @param
    //       comType: 权限类型
    //       iApp:    应用ID
    //
    // @return value
    //       0: Success
    //      -1: TimeOut
    virtual int getInteractiveAuth(
        INTERACTIVE_TYPE comType ) = 0;

    //
    //! @brief 释放交互权限
    //仅等待分配时隙的通信执行具体的操作之前要调用此阻塞接口，
    //确保已经获取到时隙区执行后续的操作
    //
    // @param
    //       comType: 权限类型
    //       iApp:    应用ID
    //
    // return value
    //       0: Success
    //      <0: Error
    virtual int releaseInteractiveAuth(
        INTERACTIVE_TYPE comType ) = 0;


    //
    //!@brief 获取某一主机节点的状态
    //
    // @param
    //       hostId: 主机节点ID
    //
    // return 主机节点的工作状态
    //        HOST_STATE
    virtual HOST_STATE getHostState( int hostId ) = 0;

    //
    // brief 组件对象的创建接口
    //       单件模式
    static IvBus* Instance( int iApp, int hostId );

};

#endif

