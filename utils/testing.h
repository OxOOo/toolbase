#ifndef TOOLBASE_UTILS_TESTING_H_
#define TOOLBASE_UTILS_TESTING_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace utils {
namespace testing {

#ifdef GTEST_HAS_STATUS_MATCHERS
using IsOk = ::testing::status::IsOk;
using IsOkAndHolds = ::testing::status::IsOkAndHolds;
using StatusIs = ::testing::status::StatusIs;
#else  // GTEST_HAS_STATUS_MATCHERS
namespace testing_internal {
inline const ::absl::Status& GetStatus(const ::absl::Status& status) {
    return status;
}

template <typename T>
inline const ::absl::Status& GetStatus(const ::absl::StatusOr<T>& status) {
    return status.status();
}
}  // namespace testing_internal

// Monomorphic implementation of matcher IsOkAndHolds(m).  StatusOrType is a
// reference to StatusOr<T>.
template <typename StatusOrType>
class IsOkAndHoldsMatcherImpl
    : public ::testing::MatcherInterface<StatusOrType> {
   public:
    typedef typename std::remove_reference<StatusOrType>::type::value_type
        value_type;

    template <typename InnerMatcher>
    explicit IsOkAndHoldsMatcherImpl(InnerMatcher&& inner_matcher)
        : inner_matcher_(::testing::SafeMatcherCast<const value_type&>(
              std::forward<InnerMatcher>(inner_matcher))) {}

    void DescribeTo(std::ostream* os) const override {
        *os << "is OK and has a value that ";
        inner_matcher_.DescribeTo(os);
    }

    void DescribeNegationTo(std::ostream* os) const override {
        *os << "isn't OK or has a value that ";
        inner_matcher_.DescribeNegationTo(os);
    }

    bool MatchAndExplain(
        StatusOrType actual_value,
        ::testing::MatchResultListener* result_listener) const override {
        if (!actual_value.ok()) {
            *result_listener << "which has status " << actual_value.status();
            return false;
        }

        ::testing::StringMatchResultListener inner_listener;
        const bool matches =
            inner_matcher_.MatchAndExplain(*actual_value, &inner_listener);
        const std::string inner_explanation = inner_listener.str();
        if (!inner_explanation.empty()) {
            *result_listener << "which contains value "
                             << ::testing::PrintToString(*actual_value) << ", "
                             << inner_explanation;
        }
        return matches;
    }

   private:
    const ::testing::Matcher<const value_type&> inner_matcher_;
};

// Implements IsOkAndHolds(m) as a polymorphic matcher.
template <typename InnerMatcher>
class IsOkAndHoldsMatcher {
   public:
    explicit IsOkAndHoldsMatcher(InnerMatcher inner_matcher)
        : inner_matcher_(std::move(inner_matcher)) {}

    // Converts this polymorphic matcher to a monomorphic matcher of the
    // given type.  StatusOrType can be either StatusOr<T> or a
    // reference to StatusOr<T>.
    template <typename StatusOrType>
    operator ::testing::Matcher<StatusOrType>() const {  // NOLINT
        return ::testing::Matcher<StatusOrType>(
            new IsOkAndHoldsMatcherImpl<const StatusOrType&>(inner_matcher_));
    }

   private:
    const InnerMatcher inner_matcher_;
};

// Monomorphic implementation of matcher IsOk() for a given type T.
// T can be Status, StatusOr<>, or a reference to either of them.
template <typename T>
class MonoIsOkMatcherImpl : public ::testing::MatcherInterface<T> {
   public:
    void DescribeTo(std::ostream* os) const override { *os << "is OK"; }
    void DescribeNegationTo(std::ostream* os) const override {
        *os << "is not OK";
    }
    bool MatchAndExplain(T actual_value,
                         ::testing::MatchResultListener*) const override {
        return testing_internal::GetStatus(actual_value).ok();
    }
};

// Implements IsOk() as a polymorphic matcher.
class IsOkMatcher {
   public:
    template <typename T>
    operator ::testing::Matcher<T>() const {  // NOLINT
        return ::testing::Matcher<T>(new MonoIsOkMatcherImpl<T>());
    }
};

// Macros for testing the results of functions that return absl::Status or
// absl::StatusOr<T> (for any type T).
#define EXPECT_OK(expression) EXPECT_THAT(expression, ::utils::testing::IsOk())
#define ASSERT_OK(expression) ASSERT_THAT(expression, ::utils::testing::IsOk())

// Returns a gMock matcher that matches a StatusOr<> whose status is
// OK and whose value matches the inner matcher.
template <typename InnerMatcher>
IsOkAndHoldsMatcher<typename std::decay<InnerMatcher>::type> IsOkAndHolds(
    InnerMatcher&& inner_matcher) {
    return IsOkAndHoldsMatcher<typename std::decay<InnerMatcher>::type>(
        std::forward<InnerMatcher>(inner_matcher));
}

// Returns a gMock matcher that matches a Status or StatusOr<> which is OK.
inline IsOkMatcher IsOk() { return IsOkMatcher(); }

template <typename T>
class StatusIsMatcherImpl : public ::testing::MatcherInterface<T> {
   public:
    StatusIsMatcherImpl(absl::StatusCode status_code)
        : status_code_(status_code) {}
    void DescribeTo(std::ostream* os) const override {
        *os << "is " << absl::StatusCodeToString(status_code_);
    }
    void DescribeNegationTo(std::ostream* os) const override {
        *os << "is not " << absl::StatusCodeToString(status_code_);
    }
    bool MatchAndExplain(T actual_value,
                         ::testing::MatchResultListener*) const override {
        return testing_internal::GetStatus(actual_value).code() == status_code_;
    }

   private:
    const absl::StatusCode status_code_;
};

class StatusIsMatcher {
   public:
    StatusIsMatcher(absl::StatusCode status_code) : status_code_(status_code) {}
    template <typename T>
    operator ::testing::Matcher<T>() const {  // NOLINT
        return ::testing::Matcher<T>(new StatusIsMatcherImpl<T>(status_code_));
    }

   private:
    const absl::StatusCode status_code_;
};

inline StatusIsMatcher StatusIs(absl::StatusCode status_code) {
    return StatusIsMatcher(status_code);
}

#endif  // GTEST_HAS_STATUS_MATCHERS

}  // namespace testing
}  // namespace utils

#endif  // TOOLBASE_UTILS_TESTING_H_