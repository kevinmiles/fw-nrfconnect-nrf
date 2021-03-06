/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <errno.h>
#include <cortex_m/tz.h>
#include <misc/reboot.h>
#include <autoconf.h>
#include <secure_services.h>

/*
 * Secure Entry functions to allow access to secure services from non-secure
 * firmware.
 *
 * Note: the function will be located in a Non-Secure
 * Callable region of the Secure Firmware Image.
 */

#ifdef CONFIG_SPM_SERVICE_RNG
#ifdef MBEDTLS_CONFIG_FILE
#include MBEDTLS_CONFIG_FILE
#else
#include "mbedtls/config.h"
#endif /* MBEDTLS_CONFIG_FILE */

#include <mbedtls/platform.h>
#include <mbedtls/entropy_poll.h>

/** @brief Structure holding the memory required to generate entropy
 */
static mbedtls_rng_workbuf_internal rng_workbuf;
#endif


int spm_secure_services_init(void)
{
	int err = 0;

#ifdef CONFIG_SPM_SERVICE_RNG
	mbedtls_platform_context platform_ctx = {
		.p_rnd_workbuf = &rng_workbuf
	};
	err = mbedtls_platform_setup(&platform_ctx);
#endif
	return err;
}


#ifdef CONFIG_SPM_SERVICE_REBOOT
__TZ_NONSECURE_ENTRY_FUNC
void spm_request_system_reboot(void)
{
	sys_reboot(SYS_REBOOT_COLD);
}
#endif /* CONFIG_SPM_SERVICE_REBOOT */


#ifdef CONFIG_SPM_SERVICE_RNG
__TZ_NONSECURE_ENTRY_FUNC
int spm_request_random_number(u8_t *output, size_t len, size_t *olen)
{
	int err;

	if (len != MBEDTLS_ENTROPY_MAX_GATHER) {
		return -EINVAL;
	}

	err = mbedtls_hardware_poll(&rng_workbuf, output, len, olen);
	return err;
}
#endif /* CONFIG_SPM_SERVICE_RNG */
