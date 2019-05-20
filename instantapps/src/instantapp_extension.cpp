#define EXTENSION_NAME InstantApp
#define LIB_NAME "InstantApp"
#define MODULE_NAME "instantapp"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)

#include <jni.h>

struct GiaState
{
    jobject                  m_GiaJNI;
    jmethodID                m_isInstantApp;
    jmethodID                m_showInstallPrompt;
    jmethodID                m_getInstantAppCookieMaxSize;
    jmethodID                m_getInstantAppCookie;
    jmethodID                m_setInstantAppCookie;
};

GiaState g_Gia;

struct ThreadAttacher {
    JNIEnv *env;
    bool has_attached;
    ThreadAttacher() : env(NULL), has_attached(false) {
        if (dmGraphics::GetNativeAndroidJavaVM()->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
            dmGraphics::GetNativeAndroidJavaVM()->AttachCurrentThread(&env, NULL);
            has_attached = true;
        }
    }
    ~ThreadAttacher() {
        if (has_attached) {
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
            }
            env->ExceptionClear();
            dmGraphics::GetNativeAndroidJavaVM()->DetachCurrentThread();
        }
    }
};

struct ClassLoader {
    private:
    JNIEnv *env;
    jobject class_loader_object;
    jmethodID find_class;
    public:
    ClassLoader(JNIEnv *env) : env(env) {
        jclass activity_class = env->FindClass("android/app/NativeActivity");
        jmethodID get_class_loader = env->GetMethodID(activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
        class_loader_object = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
        jclass class_loader = env->FindClass("java/lang/ClassLoader");
        find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        env->DeleteLocalRef(activity_class);
        env->DeleteLocalRef(class_loader);
    }
    ~ClassLoader() {
        env->DeleteLocalRef(class_loader_object);
    }
    jclass load(const char *class_name) {
        jstring str_class_name = env->NewStringUTF(class_name);
        jclass loaded_class = (jclass)env->CallObjectMethod(class_loader_object, find_class, str_class_name);
        env->DeleteLocalRef(str_class_name);
        return loaded_class;
    }
};

static int IsInstantApp(lua_State* L)
{
    int top = lua_gettop(L);
    
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    jboolean return_value = (jboolean)env->CallBooleanMethod(g_Gia.m_GiaJNI, g_Gia.m_isInstantApp);
    lua_pushboolean(L, JNI_TRUE == return_value);
    
    assert(top + 1 == lua_gettop(L));
    
    return 1;
}

static int ShowInstallPrompt(lua_State* L)
{
    int top = lua_gettop(L);
    
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    env->CallVoidMethod(g_Gia.m_GiaJNI, g_Gia.m_showInstallPrompt);
    
    assert(top == lua_gettop(L));
    
    return 0;
}

static int GetInstantAppCookieMaxSize(lua_State* L)
{
    int top = lua_gettop(L);
    
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    jint maxSize = env->CallIntMethod(g_Gia.m_GiaJNI, g_Gia.m_getInstantAppCookieMaxSize);
    lua_pushnumber(L, (int)maxSize);

    assert(top + 1 == lua_gettop(L));
    
    return 1;
}

static int GetInstantAppCookie(lua_State* L)
{
    int top = lua_gettop(L);
    
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;

    int lenght = 0;
    jbyte* cookie = NULL;

    jbyteArray cookieBArray = (jbyteArray)env->CallObjectMethod(g_Gia.m_GiaJNI, g_Gia.m_getInstantAppCookie);

    if(cookieBArray != NULL)
    {
        lenght = env->GetArrayLength(cookieBArray);
        cookie = env->GetByteArrayElements(cookieBArray, NULL);
    }
    
    lua_pushlstring(L, (const char *)cookie, lenght);
    
    env->ReleaseByteArrayElements(cookieBArray, cookie, 0);

    assert(top + 1 == lua_gettop(L));

    return 1;
}

static int SetInstantAppCookie(lua_State* L)
{
    int top = lua_gettop(L);
    
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;

    size_t cookie_lenght;
    const char* cookie = luaL_checklstring(L, 1, &cookie_lenght);

    jbyteArray cookieBArray = env->NewByteArray(cookie_lenght);
    env->SetByteArrayRegion(cookieBArray, 0, cookie_lenght, (jbyte*)cookie);
    env->CallVoidMethod(g_Gia.m_GiaJNI, g_Gia.m_setInstantAppCookie, cookieBArray);
    
    assert(top == lua_gettop(L));

    return 0;
}

static const luaL_reg intantapp_methods[] =
{
    {"show_install_prompt", ShowInstallPrompt},
    {"is_instant_app", IsInstantApp},
    {"get_cookie_max_size", GetInstantAppCookieMaxSize},
    {"get_cookie", GetInstantAppCookie},
    {"set_cookie", SetInstantAppCookie},
    {0,0}
};

static dmExtension::Result AppInitializeInstantApp(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    luaL_register(L, MODULE_NAME, intantapp_methods);
    lua_pop(L,  1);
}

static void CreateJObject()
{
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    ClassLoader class_loader = ClassLoader(env);
    jclass cls = class_loader.load("com.agulev.instantapp.GiaJNI");

    g_Gia.m_isInstantApp = env->GetMethodID(cls, "isInstantApp", "()Z");
    g_Gia.m_showInstallPrompt = env->GetMethodID(cls, "showInstallPrompt", "()V");
    g_Gia.m_getInstantAppCookieMaxSize = env->GetMethodID(cls, "getInstantAppCookieMaxSize", "()I");
    g_Gia.m_getInstantAppCookie = env->GetMethodID(cls, "getInstantAppCookie", "()[B");
    g_Gia.m_setInstantAppCookie = env->GetMethodID(cls, "setInstantAppCookie", "([B)V");

    jmethodID jni_constructor = env->GetMethodID(cls, "<init>", "(Landroid/app/Activity;)V");
    g_Gia.m_GiaJNI = env->NewGlobalRef(env->NewObject(cls, jni_constructor, dmGraphics::GetNativeAndroidActivity()));
}

static dmExtension::Result InitializeInstantApp(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    CreateJObject();
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeInstantApp(dmExtension::AppParams* params)
{
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    env->DeleteGlobalRef(g_Gia.m_GiaJNI);
    g_Gia.m_GiaJNI = NULL;

    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeInstantApp(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeInstantApp, AppFinalizeInstantApp, InitializeInstantApp, 0, 0, FinalizeInstantApp)

#else

dmExtension::Result InitializeInstantApp(dmExtension::Params* params)
{
    dmLogInfo("Registered extension InstantApp (null)");
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeInstantApp(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0,0,InitializeInstantApp, 0, 0, FinalizeInstantApp)

#endif