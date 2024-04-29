#include "MpRpcController.h"

MprpcController::MprpcController():failed_(false),errStr_("")
{}

void MprpcController::Reset()
{
    failed_ = false;
    errStr_ = "";
}

bool MprpcController::Failed() const
{
    return failed_;
}

void MprpcController::SetFailed(const std::string &reason)
{
    failed_ = true;
    errStr_ = reason;
}

std::string MprpcController::ErrorText() const
{
    return errStr_;
}

void MprpcController::StartCancel()
{
}

bool MprpcController::IsCanceled() const
{
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
}
