##############################################
# wrtbox is a Swiss Army knife for WRTnode  
# WRTnode's busybox
# This file is part of wrtbox.
# Author: 39514004@qq.com (huamanlou,alais name intel inside)
#
# This library is free software; under the terms of the GPL
#
##############################################

include $(TOPDIR)/rules.mk
PKG_NAME:=httpclient
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_BUILD_DIR:= $(BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk

define Package/httpclient
  CATEGORY:=WRTnode
  TITLE:=httpclient cli -- prints a snarky message huamanlou
  DEPENDS:=+libhttpclient +liblua
endef

define Package/libhttpclient
  CATEGORY:=WRTnode
  TITLE:=httpclient lib -- prints a snarky message huamanlou
  DEPENDS:=+libhttpclient +liblua
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/httpclient/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/httpclient $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libhttpclient.so $(1)/usr/lib/libhttpclient.so
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/httpclient.so $(1)/usr/lib/lua/    
endef
    
$(eval $(call BuildPackage,httpclient))
$(eval $(call BuildPackage,libhttpclient))
