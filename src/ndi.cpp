#include <napi.h>
#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Find.h>
#include <list>

using namespace Napi;

class FindSourcesWorker : public AsyncWorker {
    public:
        FindSourcesWorker(Napi::Env env, Napi::Promise::Deferred& deferred, double waitTime, double iterations, const char * extraIPs)
        : AsyncWorker(env), env(env), deferred(deferred), waitTime(waitTime), iterations(iterations), extraIPs(extraIPs) {}

        ~FindSourcesWorker() {}

    void Execute() override {
      NDIlib_find_create_t find_create;
      find_create.show_local_sources = true;
      find_create.p_groups = NULL;
      if (extraIPs != NULL) {
        find_create.p_groups = extraIPs;
      }

      NDIlib_find_instance_t pFind = NDIlib_find_create_v2(&find_create);
      if (pFind) {
        std::list<NDIlib_source_t> sources;

        for (uint32_t j = 0; j < iterations; j++) {
          if (NDIlib_find_wait_for_sources(pFind, waitTime)) {
            uint32_t no_sources = 0;
            const NDIlib_source_t* p_sources = NDIlib_find_get_current_sources(pFind, &no_sources);
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

        deferred.Resolve(resultArray);
        return;
      }

      deferred.Reject(Napi::TypeError::New(env, "Unknown error occured").Value());
      return;
    }

    private:
        Napi::Promise::Deferred deferred;
        Napi::Env env;
        double waitTime; 
        double iterations; 
        const char * extraIPs;
};

Napi::Promise FindMethod(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  auto deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() < 2) {
    deferred.Reject(Napi::TypeError::New(env, "Wrong number of arguments").Value());
    return deferred.Promise();
  } 

  double waitTime = info[0].As<Napi::Number>().DoubleValue();
  double iterations = info[1].As<Napi::Number>().DoubleValue();
  const char * extraIPs = info.Length() == 3 ? info[2].As<Napi::String>().Utf8Value().c_str() : NULL;

  FindSourcesWorker* worker = new FindSourcesWorker(env, deferred, waitTime, iterations, extraIPs);
  worker->Queue();
  return deferred.Promise();
}

Napi::Object InitFind(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "find"), Napi::Function::New(env, FindMethod));
  return exports;
}

NODE_API_MODULE(ndi, InitFind);