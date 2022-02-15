#include "benchmark/benchmark.h"

#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include "absl/synchronization/mutex.h"

namespace {

namespace absl_mutex {

class StringGetter {
 public:
  void Set(std::string value) {
    absl::MutexLock lock(&mutex_);
    value_ = std::move(value);
  }

  std::string Get() const {
    absl::ReaderMutexLock lock(&mutex_);
    return value_;
  }

 private:
  mutable absl::Mutex mutex_;
  std::string value_;
};

}  // namespace absl_mutex

namespace absl_mutex_ex {

class StringGetter {
 public:
  void Set(std::string value) {
    absl::MutexLock lock(&mutex_);
    value_ = std::move(value);
  }
  std::string Get() const {
    absl::MutexLock lock(&mutex_);
    return value_;
  }

 private:
  mutable absl::Mutex mutex_;
  std::string value_;
};

}  // namespace absl_mutex_ex

namespace std_mutex {

class StringGetter {
 public:
  void Set(std::string value) {
    std::unique_lock lock(mutex_);
    value_ = std::move(value);
  }

  std::string Get() const {
    std::shared_lock lock(mutex_);
    return value_;
  }

 private:
  mutable std::shared_mutex mutex_;
  std::string value_;
};

}  // namespace std_mutex

namespace std_mutex_ex {

class StringGetter {
 public:
  void Set(std::string value) {
    std::unique_lock lock(mutex_);
    value_ = std::move(value);
  }
  std::string Get() const {
    std::unique_lock lock(mutex_);
    return value_;
  }

 private:
  mutable std::mutex mutex_;
  std::string value_;
};

}  // std_mutex_ex

template <typename BenchmarkClass>
void BM_Get(benchmark::State& state) {
  static auto* value = new BenchmarkClass;
  value->Set("Hello world");
  for (auto s : state) {
    benchmark::DoNotOptimize(value->Get());
  }
}

BENCHMARK(BM_Get<absl_mutex::StringGetter>)->ThreadRange(2, 32)->Threads(512);
BENCHMARK(BM_Get<absl_mutex_ex::StringGetter>)->ThreadRange(2, 32)->Threads(512);
BENCHMARK(BM_Get<std_mutex::StringGetter>)->ThreadRange(2, 32)->Threads(512);
BENCHMARK(BM_Get<std_mutex_ex::StringGetter>)->ThreadRange(2, 32)->Threads(512);

}  // namespace

BENCHMARK_MAIN();
