#pragma once

#include <string>
#include <unordered_map>

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
    {MessageCode::kOk, "Ok"},
    {MessageCode::kBadRequest, "Bad request"},
    {MessageCode::kUnauthorized, "Unauthorized"},
    {MessageCode::kForbidden, "Forbidden"},
    {MessageCode::kNotFound, "Not found"},
    {MessageCode::kStateMismatch, "State mismatch"},
    {MessageCode::kGone, "Gone"},
    {MessageCode::kServerError, "Internal server error"}
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
