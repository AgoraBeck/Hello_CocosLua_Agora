LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d-x )
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d-x/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d-x/cocos)

include $(CLEAR_VARS)
LOCAL_MODULE := agora-cocos2dx
LOCAL_SRC_FILES := ../../../../AgoraGamingSDK/libs/android/$(TARGET_ARCH_ABI)/libagora-cocos2dx.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := agora-rtc
LOCAL_SRC_FILES := ../../../../AgoraGamingSDK/libs/android/$(TARGET_ARCH_ABI)/libagora-rtc-sdk-jni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := apm-plugin-agora-cocos2dx
LOCAL_SRC_FILES := ../../../../AgoraGamingSDK/libs/android/$(TARGET_ARCH_ABI)/libapm-plugin-agora-cocos2dx.so
include $(PREBUILT_SHARED_LIBRARY)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SRC_FILES := \
../../Classes/AppDelegate.cpp \
hellolua/main.cpp     \
../../Classes/HelloCocosLua.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../../../AgoraGamingSDK/include

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static agora-cocos2dx
LOCAL_SHARED_LIBRARIES := agora-rtc apm-plugin-agora-cocos2dx

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,scripting/lua-bindings/proj.android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
