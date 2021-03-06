#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "config.h"

namespace Quacks
{
  namespace Twit
  {
    class Account;

    class IAccountStore
    {
    public:
      using CreatingAccountResultCallback =
        std::function<void(bool, const std::string &, std::shared_ptr<Account>)>;
      virtual std::vector< std::shared_ptr<Account> > storedAccounts() = 0;
      virtual void beginCreateAccount(const CreatingAccountResultCallback &callback) = 0;
    };

#if defined(USE_CURL)
    class FileAccountStore : public IAccountStore, public std::enable_shared_from_this<FileAccountStore>
    {
    public:
      static std::shared_ptr<FileAccountStore> GetAccountStore(const std::string &filename);
      static std::shared_ptr<FileAccountStore> CreateAccountStore(const std::string &filename, const std::string &key, const std::string &secret, const std::string &pass);
      std::vector< std::shared_ptr<Account> > storedAccounts();
      void beginCreateAccount(const CreatingAccountResultCallback &callback);

      bool unlock(const std::string &pass);
      bool isLocked();
      const void *getData() const;
      void save();
    private:
      FileAccountStore(const std::string &filename);
      FileAccountStore(const std::string &filename, const std::string &key, const std::string &secret, const std::string &pass);
      class Impl;
      std::unique_ptr<Impl> impl;
      static std::map<std::string, std::shared_ptr<FileAccountStore>> storeMap;
    };
#endif

#if defined(USE_APPLE) || defined(USE_ANDROID)
    class SystemAccountStore : public IAccountStore, public std::enable_shared_from_this<SystemAccountStore>
    {
    public:
#ifdef USE_ANDROID
      static void Init(void *context);
#endif
      static SystemAccountStore *GetAccountStore();
      void requestAccess();
      bool waitGrant();
      std::vector< std::shared_ptr<Account> > storedAccounts();
      void beginCreateAccount(const CreatingAccountResultCallback &callback);

    private:
      SystemAccountStore();
      class Impl;
      static std::unique_ptr<SystemAccountStore> instance;
      std::unique_ptr<Impl> impl;
    };
#endif
  }
}
