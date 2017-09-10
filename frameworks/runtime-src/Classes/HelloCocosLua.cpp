//
//  HelloCocosLua.cpp
//  Hello_CocosLua_Agora
//
//  Created by BeckLee on 31/8/2017.
//
//

#include "HelloCocosLua.h"
#include <iostream>
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

USING_NS_CC;
AgoraEngineManager* AgoraEngineManager::_instance = NULL;


AgoraEngineManager* AgoraEngineManager::getInstance()
{
    if (NULL == AgoraEngineManager::_instance) {
        AgoraEngineManager::_instance = new AgoraEngineManager();
    }
    return AgoraEngineManager::_instance;
}

AgoraEngineManager::AgoraEngineManager()
: _localVideoSprite(nullptr)
{
    AgoraRtcEngineForGaming_getInstance()->setEventHandler(this);
}

AgoraEngineManager::~AgoraEngineManager()
{
    clear();
    for (const auto& itr : _scriptCallbacks) {
        LuaEngine::getInstance()->removeScriptHandler(itr.second);
    }
}

void AgoraEngineManager::clear() {
    CC_SAFE_RELEASE_NULL(_localVideoSprite);
    for (auto& itr : _remoteVideoSprites) {
        CC_SAFE_RELEASE(itr.second);
    }
    _remoteVideoSprites.clear();
}

LUA_FUNCTION AgoraEngineManager::scriptCallback(const std::string& eventName)
{
    auto itr = _scriptCallbacks.find(eventName);
    if (itr != _scriptCallbacks.end()) return itr->second;
    return 0;
}

#define BEGIN_LUA_CALLBACK(eventName) \
LUA_FUNCTION _callback = scriptCallback(eventName); \
if (_callback) { \
LuaStack* _stack = LuaEngine::getInstance()->getLuaStack(); \
lua_State* L = _stack->getLuaState();
#define END_LUA_CALLBACK(numArgs) \
_stack->executeFunctionByHandler(_callback, numArgs); \
}

#define BEGIN_LUA_CALLBACK_CC(eventName) \
LUA_FUNCTION _callback = scriptCallback(eventName); \
if (_callback) { \
Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] { \
LuaStack* _stack = LuaEngine::getInstance()->getLuaStack(); \
lua_State* L = _stack->getLuaState();
#define END_LUA_CALLBACK_CC(numArgs) \
_stack->executeFunctionByHandler(_callback, numArgs); \
}); \
}

#define SET_STATS(L, stats, prop) \
lua_pushliteral(L, #prop); \
lua_pushnumber(L, stats.prop); \
lua_settable(L, -3);

void AgoraEngineManager::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
    CCLOG("[Agora]:onJoinChannelSuccess %s, %u, %d", channel, uid, elapsed);
    static char channelName[0x100];
    strcpy(channelName, channel);
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
            if (_localVideoSprite == nullptr) {
                auto sprite = agora()->createSprite();
                sprite->setContentSize(Size(160, 90));
                agora()->setupLocalVideoSprite(sprite);
                sprite->retain();
                _localVideoSprite = sprite;
            }
            else {
                agora()->setupLocalVideoSprite(_localVideoSprite);
            }
        BEGIN_LUA_CALLBACK("onJoinChannelSuccess")
        lua_pushstring(L, channelName);
        lua_pushinteger(L, uid);
        lua_pushinteger(L, elapsed);
        END_LUA_CALLBACK(3)
    });
}

void AgoraEngineManager::onLeaveChannel(const RtcStats& stats) {
    CCLOG("[Agora]:onLeaveChannel %d, %d, %d", stats.totalDuration, stats.txBytes, stats.rxBytes);
    clear();
    BEGIN_LUA_CALLBACK_CC("onLeaveChannel")
    lua_newtable(L);
    SET_STATS(L, stats, totalDuration)
    SET_STATS(L, stats, txBytes)
    SET_STATS(L, stats, rxBytes)
    SET_STATS(L, stats, txKBitRate)
    SET_STATS(L, stats, rxKBitRate)
    SET_STATS(L, stats, txAudioKBitRate)
    SET_STATS(L, stats, rxAudioKBitRate)
    SET_STATS(L, stats, txVideoKBitRate)
    SET_STATS(L, stats, rxVideoKBitRate)
    SET_STATS(L, stats, users)
    SET_STATS(L, stats, cpuTotalUsage)
    SET_STATS(L, stats, cpuAppUsage)
    END_LUA_CALLBACK_CC(1)
}

void AgoraEngineManager::onUserJoined(uid_t uid, int elapsed) {
    CCLOG("[Agora]:onUserJoined uid: %u", uid);
    BEGIN_LUA_CALLBACK_CC("onUserJoined")
    lua_pushinteger(L, uid);
    lua_pushinteger(L, elapsed);
    END_LUA_CALLBACK_CC(2)
}

void AgoraEngineManager::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
    onUserEnableVideo(uid, false);
    CCLOG("[Agora]:onUserOffline uid: %u", uid);
    BEGIN_LUA_CALLBACK_CC("onUserOffline")
    lua_pushinteger(L, uid);
    lua_pushinteger(L, (int)reason);
    END_LUA_CALLBACK_CC(2)
}

void AgoraEngineManager::onAudioRouteChanged(AUDIO_ROUTE_TYPE routing) {
    CCLOG("[Agora]:onAudioRouteChanged %d", routing);
    BEGIN_LUA_CALLBACK_CC("onAudioRouteChanged")
    lua_pushinteger(L, (int)routing);
    END_LUA_CALLBACK_CC(1)
}

void AgoraEngineManager::onRequestChannelKey() {
    CCLOG("[Agora]:onRequestChannelKey");
    BEGIN_LUA_CALLBACK_CC("onRequestChannelKey")
    END_LUA_CALLBACK_CC(0)
}

void AgoraEngineManager::onUserMuteVideo(uint32_t uid, bool muted) {
    CCLOG("[Agora]:onUserMuteVideo uid:%u mute:%d", uid, muted);
    BEGIN_LUA_CALLBACK_CC("onUserMuteVideo")
    lua_pushinteger(L, uid);
    lua_pushboolean(L, muted);
    END_LUA_CALLBACK_CC(2)
}

void AgoraEngineManager::onUserEnableVideo(uint32_t uid, bool enabled) {
    CCLOG("[Agora]:onUserEnableVideo uid:%u enable:%d", uid, enabled);
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
        auto itr = _remoteVideoSprites.find(uid);
        if (itr != _remoteVideoSprites.end()) {
        if (!enabled) {
        agora()->setupRemoteVideoSprite(NULL, uid);
        CC_SAFE_RELEASE(itr->second);
        _remoteVideoSprites.erase(itr);
        }
    }

    BEGIN_LUA_CALLBACK("onUserEnableVideo")
    lua_pushinteger(L, uid);
    lua_pushboolean(L, enabled);
    END_LUA_CALLBACK(2)
    });
}

void AgoraEngineManager::onRemoteVideoStats(RemoteVideoStats& stats) {
    CCLOG("[Agora]:onRemoteVideoStats uid:%u bitrate:%d framerate:%d", stats.uid, stats.receivedBitrate, stats.receivedFrameRate);
    BEGIN_LUA_CALLBACK_CC("onRemoteVideoStats")
    lua_newtable(L);
    SET_STATS(L, stats, uid)
    SET_STATS(L, stats, delay)
    SET_STATS(L, stats, width)
    SET_STATS(L, stats, height)
    SET_STATS(L, stats, receivedBitrate)
    SET_STATS(L, stats, receivedFrameRate)
    SET_STATS(L, stats, rxStreamType)
    END_LUA_CALLBACK_CC(1)
}

void AgoraEngineManager::onLocalVideoStats(LocalVideoStats& stats) {
    CCLOG("[Agora]:onLocalVideoStats bitrate:%d framerate:%d", stats.sentBitrate, stats.sentFrameRate);
    BEGIN_LUA_CALLBACK_CC("onLocalVideoStats")
    lua_newtable(L);
    SET_STATS(L, stats, sentBitrate)
    SET_STATS(L, stats, sentFrameRate)
    END_LUA_CALLBACK_CC(1)
}

void AgoraEngineManager::onFirstRemoteVideoFrame(uint32_t uid, int width, int height, int elapsed) {
    CCLOG("[Agora]:onFirstRemoteVideoFrame uid:%d width:%d height:%d", uid, width, height);
    BEGIN_LUA_CALLBACK_CC("onFirstRemoteVideoFrame")
    lua_pushinteger(L, uid);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    lua_pushinteger(L, elapsed);
    END_LUA_CALLBACK_CC(4)
}

void AgoraEngineManager::onFirstLocalVideoFrame(uint32_t width, int height, int elapsed) {
    CCLOG("[Agora]:onFirstLocalVideoFram width:%d height:%d", width, height);
    BEGIN_LUA_CALLBACK_CC("onFirstLocalVideoFrame")
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    lua_pushinteger(L, elapsed);
    END_LUA_CALLBACK_CC(3)
}

void AgoraEngineManager::onFirstRemoteVideoDecoded(uint32_t uid, int width, int height, int elapsed)  {
    CCLOG("[Agora]:onFirstRemoteVideoDecoded uid:%u, width:%d height:%d", uid, width, height);
    if (1){
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
            auto sprite = remoteVideoSprite(uid);
            if (sprite == nullptr) {
                sprite = agora()->createSprite();
                sprite->setContentSize(Size(160, 90));
                sprite->retain();
                _remoteVideoSprites[uid] = sprite;
            }
            
            agora()->setupRemoteVideoSprite(sprite, uid);
        
            BEGIN_LUA_CALLBACK("onFirstRemoteVideoDecoded")
            lua_pushinteger(L, uid);
            lua_pushinteger(L, width);
            lua_pushinteger(L, height);
            lua_pushinteger(L, elapsed);
            END_LUA_CALLBACK(4)
        });
    }
}

void AgoraEngineManager::onVideoSizeChanged(uint32_t uid, int width, int height, int rotation) {
    BEGIN_LUA_CALLBACK_CC("onVideoSizeChanged")
    lua_pushinteger(L, uid);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    lua_pushinteger(L, rotation);
    END_LUA_CALLBACK_CC(4)
}

void AgoraEngineManager::onCameraReady() {
    BEGIN_LUA_CALLBACK_CC("onCameraReady")
    END_LUA_CALLBACK_CC(0)
}

void AgoraEngineManager::onVideoStopped() {
    CCLOG("[Agora]:onVideoStopped");
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([&] {
        /*if (_localVideoSprite != nullptr) {
         CCLOG("[Agora]:onVideoStopped : remove LocalVideo");
         agora()->setupLocalVideoSprite(NULL);
         }*/
        BEGIN_LUA_CALLBACK("onVideoStopped")
        END_LUA_CALLBACK(0)
    });
}

#define AGORA_CHECK(error) { int code = error; if (code != 0) CCLOG("[Agora Error] %s", agora()->getErrorDescription(code)); }
int AgoraEngineManager::joinChannel(const std::string& channelName, uint32_t uid, bool videoEnabled, int videoMode, const std::string& info)
{
    AGORA_CHECK(agora()->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING));
    AGORA_CHECK(agora()->setClientRole(CLIENT_ROLE_BROADCASTER));
    AGORA_CHECK(agora()->enableVideo(videoEnabled));
    AGORA_CHECK(agora()->setVideoProfile((VIDEO_PROFILE)videoMode, false));
    
    int error = agora()->joinChannel(channelName.c_str(), info.empty() ? "xxx" : info.c_str(), uid);
    return error;
}

int AgoraEngineManager::leaveChannel()
{
    AGORA_CHECK(agora()->stopPreview());
    int error = agora()->leaveChannel();
    
    if (_localVideoSprite != nullptr) {
        agora()->setupLocalVideoSprite(NULL);
        CC_SAFE_RELEASE_NULL(_localVideoSprite);
    }
    for (auto& itr : _remoteVideoSprites) {
        agora()->setupRemoteVideoSprite(NULL, itr.first);
        CC_SAFE_RELEASE(itr.second);
    }
    _remoteVideoSprites.clear();
    return error;
}

std::set<std::string> AgoraEngineManager::_agoraCallbackNameTable = {
    "onJoinChannelSuccess",
    "onLeaveChannel",
    "onUserJoined",
    "onUserOffline",
    "onAudioRouteChanged",
    "onRequestChannelKey",
    "onUserMuteVideo",
    "onUserEnableVideo",
    "onRemoteVideoStats",
    "onLocalVideoStats",
    "onFirstRemoteVideoFrame",
    "onFirstLocalVideoFrame",
    "onFirstRemoteVideoDecoded",
    "onVideoSizeChanged",
    "onCameraReady",
    "onVideoStopped",
};
bool AgoraEngineManager::setCallback(const std::string& eventName, LUA_FUNCTION handler)
{
    if (_agoraCallbackNameTable.count(eventName) == 0) {
        return false;
    }
    auto itr = _scriptCallbacks.find(eventName);
    if (itr != _scriptCallbacks.end()) {
        LuaEngine::getInstance()->removeScriptHandler(itr->second);
        _scriptCallbacks.erase(itr);
    }
    if (handler > 0) {
        _scriptCallbacks[eventName] = handler;
    }
    return true;
}

/*
 Agora Engine lua interface
 */

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

int lua_Agora_getLocalSprite(lua_State *L)
{
    auto sprite = AgoraEngineManager::getInstance()->localVideoSprite();
    object_to_luaval<cocos2d::Sprite>(L, "cc.Sprite", sprite);
    return 1;
}

int lua_Agora_getRemoteSprite(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 1) {
        luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Agora_getRemoteSprite", argc, 1);
        return 0;
    }
    
    uint32_t uid = static_cast<uint32_t>(static_cast<int32_t>(luaL_checknumber(L, 1)));
    auto sprite = AgoraEngineManager::getInstance()->remoteVideoSprite(uid);
    object_to_luaval<cocos2d::Sprite>(L, "cc.Sprite", sprite);
    return 1;
}

int lua_Agora_joinChannel(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 5) {
        luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Agora_joinChannel", argc, 4);
        return 0;
    }
    
    std::string channelName;
    if (!luaval_to_std_string(L, 2, &channelName, "lua_Agora_joinChannel")) {
        tolua_error(L, "invalid arguments in function 'lua_Agora_joinChannel'", nullptr);
        return 0;
    }
    
    uint32_t uid = static_cast<uint32_t>(static_cast<int32_t>(luaL_checknumber(L, 3)));
    bool videoEnabled = lua_toboolean(L, 4);
    int videoMode = luaL_checknumber(L, 5);
    std::string info = "";
    if (argc == 6) {
        luaval_to_std_string(L, 1, &info, "lua_Agora_joinChannel");
    }
    int error = AgoraEngineManager::getInstance()->joinChannel(channelName, uid, videoEnabled, videoMode, info);
    lua_pushnumber(L, error);
    return 1;
}

#if 0
int lua_Agora_joinChannel(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 5) {
        luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Agora_joinChannel", argc, 4);
        return 0;
    }
    
    std::string channelName;
    if (!luaval_to_std_string(L, 2, &channelName, "lua_Agora_joinChannel")) {
        tolua_error(L, "invalid arguments in function 'lua_Agora_joinChannel'", nullptr);
        return 0;
    }
    
    uint32_t uid = static_cast<uint32_t>(static_cast<int32_t>(luaL_checknumber(L, 3)));
    bool videoEnabled = lua_toboolean(L, 4);
    int videoMode = luaL_checknumber(L, 6);
    std::string info = "";
    if (argc == 6) {
        luaval_to_std_string(L, 6, &info, "lua_Agora_joinChannel");
    }
    int error = AgoraEngineManager::getInstance()->joinChannel(channelName, uid, videoEnabled, videoMode, info);
    lua_pushnumber(L, error);
    return 1;
}
#endif

int lua_Agora_leaveChannel(lua_State *L)
{
    int error = AgoraEngineManager::getInstance()->leaveChannel();
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_setCallback(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc < 2) {
        luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "lua_Agora_setCallback", argc, 2);
        return 0;
    }
    std::string eventName;
    
    if (!luaval_to_std_string(L, 1, &eventName, "lua_Agora_setCallback")) {
        tolua_error(L, "invalid arguments in function 'lua_Agora_setCallback'", nullptr);
        
        return 0;
    }
    LUA_FUNCTION handler = toluafix_ref_function(L, 2, 0);
    if (!AgoraEngineManager::getInstance()->setCallback(eventName, handler)) {
        luaL_error(L, "invalid callback name '%s' \n", "lua_Agora_setCallback", eventName.c_str());
        return 0;
    }
    return 0;
}

int lua_Agora_getVersion(lua_State *L)
{
    int build = 0;
    const char* buildver = AgoraEngineManager::getInstance()->agora()->getVersion(&build);
    lua_pushstring(L, buildver);
    return 1;
}

int lua_Agora_getErrorDescription(lua_State *L)
{
    int code = luaL_checknumber(L, 1);
    const char* description = AgoraEngineManager::getInstance()->agora()->getErrorDescription(code);
    lua_pushstring(L, description);
    return 1;
}

int lua_Agora_setLogFilter(lua_State *L)
{
    uint32_t filter = luaL_checknumber(L, 1);
    int error = AgoraEngineManager::getInstance()->agora()->setLogFilter(filter);
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_setLogFile(lua_State *L)
{
    const char* file = luaL_checkstring(L, 1);
    int error = AgoraEngineManager::getInstance()->agora()->setLogFile(file);
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_setParameters(lua_State *L)
{
    const char* json = luaL_checkstring(L, 1);
    int error = AgoraEngineManager::getInstance()->agora()->setParameters(json);
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_enableAudio(lua_State *L)
{
    bool enabled = lua_toboolean(L, 1);
    int error = enabled ? AgoraEngineManager::getInstance()->agora()->enableAudio() : AgoraEngineManager::getInstance()->agora()->disableAudio();
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_enableVideo(lua_State *L)
{
    bool enabled = lua_toboolean(L, 1);
    int error = AgoraEngineManager::getInstance()->agora()->enableVideo(enabled);
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_pause(lua_State *L)
{
    AgoraEngineManager::getInstance()->agora()->pause();
    return 0;
}

int lua_Agora_resume(lua_State *L)
{
    AgoraEngineManager::getInstance()->agora()->resume();
    return 0;
}

int lua_Agora_poll(lua_State *L)
{
    AgoraEngineManager::getInstance()->agora()->poll();
    return 0;
}

int lua_Agora_startPreview(lua_State *L)
{
    int error = AgoraEngineManager::getInstance()->agora()->startPreview();
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_stopPreview(lua_State *L)
{
    int error = AgoraEngineManager::getInstance()->agora()->stopPreview();
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_switchCamera(lua_State *L)
{
    int error = AgoraEngineManager::getInstance()->agora()->switchCamera();
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_muteLocalAudio(lua_State *L)
{
    bool mute = lua_toboolean(L, 1);
    int error = AgoraEngineManager::getInstance()->agora()->muteLocalAudioStream(mute);
    lua_pushnumber(L, error);
    return 1;
}

int lua_Agora_log(lua_State *L)
{
    const char* msg = luaL_checkstring(L, 1);
    CCLOG("Agora Debug :", "%s", msg);
    return 0;
}


int lua_Agora_exit(lua_State *L)
{
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
    #endif
    
    return 0;
}

static luaL_Reg Agora_func[] = {
    { "getLocalSprite", lua_Agora_getLocalSprite },
    { "getRemoteSprite", lua_Agora_getRemoteSprite },
    { "joinChannel", lua_Agora_joinChannel },
    { "leaveChannel", lua_Agora_leaveChannel },
    { "setCallback", lua_Agora_setCallback },
    
    { "getVersion", lua_Agora_getVersion },
    { "getErrorDescription", lua_Agora_getErrorDescription },
    { "setLogFilter", lua_Agora_setLogFilter },
    { "setLogFile", lua_Agora_setLogFile },
    { "setParameters", lua_Agora_setParameters },
    { "enableAudio", lua_Agora_enableAudio },
    { "enableVideo", lua_Agora_enableVideo },
    
    { "pause", lua_Agora_pause },
    { "resume", lua_Agora_resume },
    { "poll", lua_Agora_poll },
    { "startPreview", lua_Agora_startPreview },
    { "stopPreview", lua_Agora_stopPreview },
    { "switchCamera", lua_Agora_switchCamera },
    
    { "muteLocalAudio", lua_Agora_muteLocalAudio },
    { "log", lua_Agora_log },
    { "exit", lua_Agora_exit},
    { NULL, NULL }
};


int lua_Agora_register(lua_State* L)
{
    luaL_openlib(L, "Agora", Agora_func, 0);
    return 0;
}
