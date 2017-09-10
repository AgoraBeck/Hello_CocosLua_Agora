//
//  HelloCocosLua.h
//  Hello_CocosLua_Agora
//
//  Created by BeckLee on 31/8/2017.
//
//

#ifndef HelloCocosLua_h
#define HelloCocosLua_h

#include <stdio.h>

#include "../../../AgoraGamingSDK/include/agora_cocos2dx.h"

#include "cocos2d.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

USING_NS_CC;
using namespace agora::rtc::cocos2dx;

class AgoraEngineManager : public IAgoraCocos2dxEngineEventHandler {
public:
    static AgoraEngineManager* getInstance();
    AgoraEngineManager();
    ~AgoraEngineManager();
    inline IAgoraCocos2dxEngine* agora() {
        return AgoraRtcEngineForGaming_getInstance();
    }
    
public:
    Sprite* localVideoSprite() { return _localVideoSprite; }
    Sprite* remoteVideoSprite(uint32_t uid) {
        auto itr = _remoteVideoSprites.find(uid);
        return itr != _remoteVideoSprites.end() ? itr->second : nullptr;
    }
    int joinChannel(const std::string& channelName, uint32_t uid, bool videoEnabled, int videoMode, const std::string& info);
    int leaveChannel();
    bool setCallback(const std::string& eventName, LUA_FUNCTION handler);
    
private:
    /* IAgoraCocos2dxEngineEventHandler callbacks */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed);
    virtual void onLeaveChannel(const RtcStats& stats);
    virtual void onUserJoined(uid_t uid, int elapsed);
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason);
    virtual void onAudioRouteChanged(AUDIO_ROUTE_TYPE routing);
    virtual void onRequestChannelKey();
    virtual void onUserMuteVideo(uint32_t uid, bool muted);
    virtual void onUserEnableVideo(uint32_t  uid, bool enabled);
    virtual void onRemoteVideoStats(RemoteVideoStats& stats);
    virtual void onLocalVideoStats(LocalVideoStats& stats);
    virtual void onFirstRemoteVideoFrame(uint32_t uid, int width, int height, int elapsed);
    virtual void onFirstLocalVideoFrame(uint32_t width, int height, int elapsed);
    virtual void onFirstRemoteVideoDecoded(uint32_t uid, int width, int height, int elapsed);
    virtual void onVideoSizeChanged(uint32_t uid, int width, int height, int rotation);
    virtual void onCameraReady();
    virtual void onVideoStopped();
private:
    void clear();
    LUA_FUNCTION scriptCallback(const std::string& eventName);
    Sprite* _localVideoSprite;
    std::map<uint32_t, Sprite*> _remoteVideoSprites;
    std::map<std::string, LUA_FUNCTION> _scriptCallbacks;
private:
    static std::set<std::string> _agoraCallbackNameTable;
    static AgoraEngineManager* _instance;
    
};

TOLUA_API int lua_Agora_register(lua_State* L);


#endif /* HelloCocosLua_h */
