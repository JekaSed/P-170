#pragma once

namespace network {

static constexpr int statusOk = 200;
static constexpr int statusCreated = 201;
static constexpr int statusAccepted = 202;
static constexpr int statusNoContent = 204;             // RFC 7231, 6.3.5
static constexpr int statusBadRequest = 400;            // RFC 7231, 6.5.1
static constexpr int statusUnauthorized = 401;          // RFC 7235, 3.1
static constexpr int statusNotFound = 404;              // RFC 7231, 6.5.4
static constexpr int statusMethodNotAllowed = 405;      // RFC 7231, 6.5.5
static constexpr int statusNotAcceptable = 406;         // RFC 7231, 6.5.6
static constexpr int statusRequestTimeout = 408;        // RFC 7231, 6.5.7
static constexpr int statusUnprocessableEntity = 422;   // RFC 4918, 11.2
static constexpr int statusInternalServerError = 500;

}   // namespace network
