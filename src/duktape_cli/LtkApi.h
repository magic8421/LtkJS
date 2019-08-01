#pragma once

namespace LtkApi
{

    void RunMessageLoop();
    void PostQuitMessage();

    duk_ret_t GetConstantTable(duk_context *ctx);
}
