#pragma once

#include "CesiumAsync/Future.h"
#include "CesiumAsync/Impl/RemoveFuture.h"
#include "CesiumAsync/Impl/WithTracing.h"
#include "CesiumAsync/Impl/cesium-async++.h"
#include "CesiumAsync/Library.h"
#include "CesiumUtility/Profiler.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace CesiumAsync {
class ITaskProcessor;

template <typename T> class Future;

/**
 * @brief A system for managing asynchronous requests and tasks.
 *
 * Instances of this class may be safely and efficiently stored and passed
 * around by value.
 */
class CESIUMASYNC_API AsyncSystem final {
public:
  /**
   * @brief A promise that can be resolved or rejected by an asynchronous task.
   *
   * @tparam T The type of the object that the promise will be resolved with.
   */
  template <typename T> struct Promise {
    Promise(const std::shared_ptr<async::event_task<T>>& pEvent)
        : _pEvent(pEvent) {}

    /**
     * @brief Will be called when the task completed successfully.
     *
     * @param value The value that was computed by the asynchronous task.
     */
    void resolve(T&& value) const { this->_pEvent->set(std::move(value)); }

    /**
     * @brief Will be called when the task failed.
     *
     * @param error The error that caused the task to fail.
     */
    void reject(std::exception&& error) const {
      this->_pEvent->set_exception(std::make_exception_ptr(error));
    }

  private:
    std::shared_ptr<async::event_task<T>> _pEvent;
  };

  /**
   * @brief Constructs a new instance.
   *
   * @param pTaskProcessor The interface used to run tasks in background
   * threads.
   */
  AsyncSystem(const std::shared_ptr<ITaskProcessor>& pTaskProcessor) noexcept;

  template <typename T, typename Func> Future<T> createFuture(Func&& f) const {
    std::shared_ptr<async::event_task<T>> pEvent =
        std::make_shared<async::event_task<T>>();

    Promise<T> promise(pEvent);
    f(promise);

    return Future<T>(this->_pSchedulers, pEvent->get_task());
  }

  /**
   * @brief Runs a function in a worker thread, returning a promise that
   * resolves when the function completes.
   *
   * If the function itself returns a `Future`, the function will not be
   * considered complete until that returned `Future` also resolves.
   *
   * @tparam Func The type of the function.
   * @param f The function.
   * @return A future that resolves after the supplied function completes.
   */
  template <class Func>
  Future<typename Impl::RemoveFuture<
      typename std::invoke_result<Func>::type>::type>
  runInWorkerThread(Func&& f) const {
#if TRACING_ENABLED
    static const char* tracingName = "waiting for worker thread";
    int64_t tracingID = CesiumUtility::Profiler::instance().getEnlistedID();
    TRACE_ASYNC_BEGIN_ID(tracingName, tracingID);
#endif

    return Future<typename Impl::RemoveFuture<
        typename std::invoke_result<Func>::type>::type>(
        this->_pSchedulers,
        async::spawn(
            this->_pSchedulers->workerThreadScheduler,
            Impl::WithTracing<Func, void>::wrap(
                tracingName,
                std::forward<Func>(f))));
  }

  /**
   * @brief Runs a function in the main thread, returning a promise that
   * resolves when the function completes.
   *
   * The supplied function will not be called immediately, even if this method
   * is invoked from the main thread. Instead, it will be queued and called the
   * next time {@link dispatchMainThreadTasks} is called.
   *
   * If the function itself returns a `Future`, the function will not be
   * considered complete until that returned `Future` also resolves.
   *
   * @tparam Func The type of the function.
   * @param f The function.
   * @return A future that resolves after the supplied function completes.
   */
  template <class Func>
  Future<typename Impl::RemoveFuture<
      typename std::invoke_result<Func>::type>::type>
  runInMainThread(Func&& f) const {
#if TRACING_ENABLED
    static const char* tracingName = "waiting for main thread";
    int64_t tracingID = CesiumUtility::Profiler::instance().getEnlistedID();
    TRACE_ASYNC_BEGIN_ID(tracingName, tracingID);
#endif

    return Future<typename Impl::RemoveFuture<
        typename std::invoke_result<Func>::type>::type>(
        this->_pSchedulers,
        async::spawn(
            this->_pSchedulers->mainThreadScheduler,
            Impl::WithTracing<Func, T>::wrap(
                tracingName,
                std::forward<Func>(f))));
  }

  /**
   * @brief Creates a future that is already resolved.
   *
   * @tparam T The type of the future
   * @param value The value for the future
   * @return The future
   */
  template <typename T> Future<T> createResolvedFuture(T&& value) const {
    return Future<T>(
        this->_pSchedulers,
        async::make_task<T>(std::forward<T>(value)));
  }

  Future<void> createResolvedFuture() const {
    return Future<void>(this->_pSchedulers, async::make_task());
  }

  /**
   * @brief Runs all tasks that are currently queued for the main thread.
   *
   * The tasks are run in the calling thread.
   */
  void dispatchMainThreadTasks();

private:
  std::shared_ptr<Impl::AsyncSystemSchedulers> _pSchedulers;

  template <typename T> friend class Future;
};
} // namespace CesiumAsync
