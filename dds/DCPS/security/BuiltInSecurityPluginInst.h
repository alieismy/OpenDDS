/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#ifndef OPENDDS_DCPS_BUILTIN_SECURITY_INST_H
#define OPENDDS_DCPS_BUILTIN_SECURITY_INST_H

#include "dds/DCPS/security/DdsSecurity_Export.h"
#include "dds/DCPS/security/framework/SecurityPluginInst.h"

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL

namespace OpenDDS {
namespace Security {

/**
 * @class BuiltInSecurityPluginInst
 *
 * @brief Factory object to create interfaces for the BuiltIn plugin.
 */
class DdsSecurity_Export BuiltInSecurityPluginInst : public SecurityPluginInst {
public:

  BuiltInSecurityPluginInst();
  ~BuiltInSecurityPluginInst();

  virtual Authentication_var create_authentication();
  virtual AccessControl_var create_access_control();
  virtual CryptoKeyFactory_var create_crypto_key_factory();
  virtual CryptoKeyExchange_var create_crypto_key_exchange();
  virtual CryptoTransform_var create_crypto_transform();

  virtual void shutdown();

private:
  Authentication_var authentication_;
  AccessControl_var access_control_;
  CryptoKeyFactory_var key_factory_;
  CryptoKeyExchange_var key_exchange_;
  CryptoTransform_var transform_;

  BuiltInSecurityPluginInst(const BuiltInSecurityPluginInst&);
  BuiltInSecurityPluginInst& operator=(const BuiltInSecurityPluginInst&);
};


} // namespace DCPS
} // namespace OpenDDS

OPENDDS_END_VERSIONED_NAMESPACE_DECL

#endif
