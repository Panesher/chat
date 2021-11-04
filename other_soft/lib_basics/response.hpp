#pragma once

#include <string>
#include <unordered_map>

namespace lib_basics {
namespace response {

enum MessageCode {
  kOk = 200,
  kBadRequest = 400,
  kUnauthorized = 401,
  kForbidden = 403,
  kNotFound = 404,
  kStateMismatch = 409,
  kGone = 410,
  kServerError = 500,
};

namespace {

const std::unordered_map<MessageCode, std::string> MessageMap = {
    {MessageCode::kOk, "ok"},
    {MessageCode::kBadRequest, "bad request"},
    {MessageCode::kUnauthorized, "unauthorized"},
    {MessageCode::kForbidden, "forbidden"},
    {MessageCode::kNotFound, "not found"},
    {MessageCode::kStateMismatch, "state mismatch"},
    {MessageCode::kGone, "gone"},
    {MessageCode::kServerError, "internal server error"}
};

} // namespace

struct Response {
  MessageCode status = kOk;

  std::string AsString() const {
    try {
      return MessageMap.at(status);
    } catch (const std::out_of_range& e) {
      return MessageMap.at(kServerError);
    }
  }
};

} // namespace Response
} // namespace lib_basics