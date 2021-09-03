#include <napi.h>
#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Find.h>
#include <list>

Napi::Array FindMethod(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return Napi::Array::New(env);
  }

  double waitTime = info[0].As<Napi::Number>().DoubleValue();
  double iterations = info[1].As<Napi::Number>().DoubleValue();
  
  NDIlib_find_create_t find_create;
  find_create.show_local_sources = true;
  find_create.p_groups = NULL;

  if (info.Length() == 3) {
    const char * extraIPs = info[2].As<Napi::String>().Utf8Value().c_str();
    find_create.p_extra_ips = extraIPs;
  }

  NDIlib_find_instance_t pFind = NDIlib_find_create_v2(&find_create);
  if (pFind) {
    std::list<NDIlib_source_t> sources;

    for (uint32_t j = 0; j < iterations; j++) {
      if (NDIlib_find_wait_for_sources(pFind, waitTime)) {
        uint32_t no_sources = 0;
        const NDIlib_source_t* p_sources = NDIlib_find_get_current_sources(pFind, &no_sources);
        printf("Network sources (%u found).\n", no_sources);
        for (uint32_t i = 0; i < no_sources; i++) {
          sources.push_front(p_sources[i]);
        }
      }
    }

    Napi::Array resultArray = Napi::Array::New(env, sources.size());
    uint32_t i = 0;
    for (auto iter = sources.begin(); iter != sources.end(); iter++)
    {
      Napi::Object obj = Napi::Object::New(env);
      obj.Set("name", iter->p_ndi_name);
      obj.Set("urlAddress",  iter->p_url_address);

      resultArray[i] = obj;
      i++;
    }

    return resultArray;
  }

  return Napi::Array::New(env);
}

Napi::Object InitFind(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "find"), Napi::Function::New(env, FindMethod));
  return exports;
}

NODE_API_MODULE(find, InitFind);