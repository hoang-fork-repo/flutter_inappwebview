#include <WebView2EnvironmentOptions.h>
#include <wil/wrl.h>

#include "../utils/log.h"
#include "webview_environment.h"

#include "webview_environment_manager.h"

namespace flutter_inappwebview_plugin
{
  using namespace Microsoft::WRL;

  WebViewEnvironment::WebViewEnvironment(const FlutterInappwebviewWindowsPlugin* plugin, const std::string& id)
    : plugin(plugin), id(id),
    channelDelegate(std::make_unique<WebViewEnvironmentChannelDelegate>(this, plugin->registrar->messenger()))
  {}

  void WebViewEnvironment::create(const std::unique_ptr<WebViewEnvironmentSettings> settings, const std::function<void(HRESULT)> completionHandler)
  {
    if (!plugin) {
      if (completionHandler) {
        completionHandler(E_FAIL);
      }
      return;
    }

    auto hwnd = plugin->webViewEnvironmentManager->getHWND();
    if (!hwnd) {
      if (completionHandler) {
        completionHandler(E_FAIL);
      }
      return;
    }

    auto options = Make<CoreWebView2EnvironmentOptions>();
    if (settings) {
      if (settings->additionalBrowserArguments.has_value()) {
        options->put_AdditionalBrowserArguments(utf8_to_wide(settings->additionalBrowserArguments.value()).c_str());
      }
      if (settings->allowSingleSignOnUsingOSPrimaryAccount.has_value()) {
        options->put_AllowSingleSignOnUsingOSPrimaryAccount(settings->allowSingleSignOnUsingOSPrimaryAccount.value());
      }
      if (settings->language.has_value()) {
        options->put_Language(utf8_to_wide(settings->language.value()).c_str());
      }
      if (settings->targetCompatibleBrowserVersion.has_value()) {
        options->put_TargetCompatibleBrowserVersion(utf8_to_wide(settings->targetCompatibleBrowserVersion.value()).c_str());
      }
      wil::com_ptr<ICoreWebView2EnvironmentOptions4> options4;
      if (succeededOrLog(options->QueryInterface(IID_PPV_ARGS(&options4))) && settings->customSchemeRegistrations.has_value()) {
        std::vector<ICoreWebView2CustomSchemeRegistration*> registrations = {};
        for (auto& customSchemeRegistration : settings->customSchemeRegistrations.value()) {
          registrations.push_back(std::move(customSchemeRegistration->toWebView2CustomSchemeRegistration()));
        }
        options4->SetCustomSchemeRegistrations(static_cast<UINT32>(registrations.size()), registrations.data());
      }
    }

    auto hr = CreateCoreWebView2EnvironmentWithOptions(
      settings && settings->browserExecutableFolder.has_value() ? utf8_to_wide(settings->browserExecutableFolder.value()).c_str() : nullptr,
      settings && settings->userDataFolder.has_value() ? utf8_to_wide(settings->userDataFolder.value()).c_str() : nullptr,
      options.Get(),
      Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
        [this, hwnd, completionHandler](HRESULT result, wil::com_ptr<ICoreWebView2Environment> environment) -> HRESULT
        {
          if (succeededOrLog(result)) {
            environment_ = std::move(environment);

            auto hr = environment_->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
              [this, completionHandler](HRESULT result, wil::com_ptr<ICoreWebView2Controller> controller) -> HRESULT
              {
                if (succeededOrLog(result)) {
                  webViewController_ = std::move(controller);
                  webViewController_->get_CoreWebView2(&webView_);
                  webViewController_->put_IsVisible(false);
                }
                if (completionHandler) {
                  completionHandler(result);
                }
                return S_OK;
              }).Get());

            if (failedAndLog(hr) && completionHandler) {
              completionHandler(hr);
            }
          }
          else if (completionHandler) {
            completionHandler(result);
          }
          return S_OK;
        }).Get());

    if (failedAndLog(hr) && completionHandler) {
      completionHandler(hr);
    }
  }

  bool WebViewEnvironment::isInterfaceSupported(const std::string& interfaceName) const
  {
    if (!environment_) {
      return false;
    }

    if (starts_with(interfaceName, std::string{ "ICoreWebView2Environment" })) {
      switch (string_hash(interfaceName)) {
      case string_hash("ICoreWebView2Environment"):
        return environment_.try_query<ICoreWebView2Environment>() != nullptr;
      case string_hash("ICoreWebView2Environment2"):
        return environment_.try_query<ICoreWebView2Environment2>() != nullptr;
      case string_hash("ICoreWebView2Environment3"):
        return environment_.try_query<ICoreWebView2Environment3>() != nullptr;
      case string_hash("ICoreWebView2Environment4"):
        return environment_.try_query<ICoreWebView2Environment4>() != nullptr;
      case string_hash("ICoreWebView2Environment5"):
        return environment_.try_query<ICoreWebView2Environment5>() != nullptr;
      case string_hash("ICoreWebView2Environment6"):
        return environment_.try_query<ICoreWebView2Environment6>() != nullptr;
      case string_hash("ICoreWebView2Environment7"):
        return environment_.try_query<ICoreWebView2Environment7>() != nullptr;
      case string_hash("ICoreWebView2Environment8"):
        return environment_.try_query<ICoreWebView2Environment8>() != nullptr;
      case string_hash("ICoreWebView2Environment9"):
        return environment_.try_query<ICoreWebView2Environment9>() != nullptr;
      case string_hash("ICoreWebView2Environment10"):
        return environment_.try_query<ICoreWebView2Environment10>() != nullptr;
      case string_hash("ICoreWebView2Environment11"):
        return environment_.try_query<ICoreWebView2Environment11>() != nullptr;
      case string_hash("ICoreWebView2Environment12"):
        return environment_.try_query<ICoreWebView2Environment12>() != nullptr;
      case string_hash("ICoreWebView2Environment13"):
        return environment_.try_query<ICoreWebView2Environment13>() != nullptr;
      case string_hash("ICoreWebView2Environment14"):
        return environment_.try_query<ICoreWebView2Environment14>() != nullptr;
      default:
        return false;
      }
    }

    return false;
  }

  WebViewEnvironment::~WebViewEnvironment()
  {
    debugLog("dealloc WebViewEnvironment");
  }
}
