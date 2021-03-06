#pragma once

// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "ledger/LedgerManager.h"
#include "overlay/StellarXDR.h"
#include "util/types.h"
#include <memory>

namespace medida
{
class MetricsRegistry;
}

namespace stellar
{
class AbstractLedgerState;
class Application;
class LedgerManager;
class LedgerStateEntry;
class LedgerStateHeader;

class SignatureChecker;
class TransactionFrame;

enum class ThresholdLevel
{
    LOW,
    MEDIUM,
    HIGH
};

class OperationFrame
{
  protected:
    Operation const& mOperation;
    TransactionFrame& mParentTx;
    OperationResult& mResult;

    virtual bool doCheckValid(Application& app, uint32_t ledgerVersion) = 0;
    virtual bool doApply(Application& app, AbstractLedgerState& ls) = 0;

    // returns the threshold this operation requires
    virtual ThresholdLevel getThresholdLevel() const;

    // returns true if the operation is supported given a protocol version
    virtual bool isVersionSupported(uint32_t protocolVersion) const;

    LedgerStateEntry loadSourceAccount(AbstractLedgerState& ls,
                                       LedgerStateHeader const& header);

  public:
    static std::shared_ptr<OperationFrame>
    makeHelper(Operation const& op, OperationResult& res,
               TransactionFrame& parentTx);

    OperationFrame(Operation const& op, OperationResult& res,
                   TransactionFrame& parentTx);
    OperationFrame(OperationFrame const&) = delete;
    virtual ~OperationFrame() = default;

    bool checkSignature(SignatureChecker& signatureChecker, Application& app,
                        AbstractLedgerState& ls, bool forApply);

    AccountID const& getSourceID() const;

    OperationResult&
    getResult() const
    {
        return mResult;
    }
    OperationResultCode getResultCode() const;

    bool checkValid(SignatureChecker& signatureChecker, Application& app,
                    AbstractLedgerState& lsOuter, bool forApply);

    bool apply(SignatureChecker& signatureChecker, Application& app,
               AbstractLedgerState& ls);

    Operation const&
    getOperation() const
    {
        return mOperation;
    }
};
}
