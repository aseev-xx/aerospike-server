/*
 * vmapx.h
 *
 * Copyright (C) 2012-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */

#pragma once


#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct vhash_s vhash;

// DO NOT access this member data directly - use the API!
// Caution - changing this struct could break warm restart.
typedef struct cf_vmapx_s {
	// vector-related
	uint32_t			value_size;
	uint32_t			max_count;
	volatile uint32_t	count;

	// hash-related
	uint32_t			key_size;
	vhash*				p_hash;

	// generic
	pthread_mutex_t		write_lock;

	//<><><><><><><><><><><> 64 bytes <><><><><><><><><><><>

	// vector data
	uint8_t				values[];
} cf_vmapx;

typedef enum {
	CF_VMAPX_OK = 0,
	CF_VMAPX_ERR_BAD_PARAM,
	CF_VMAPX_ERR_FULL,
	CF_VMAPX_ERR_NAME_EXISTS,
	CF_VMAPX_ERR_NAME_NOT_FOUND,
	CF_VMAPX_ERR_UNKNOWN
} cf_vmapx_err;


//------------------------------------------------
// Public API.
//

size_t cf_vmapx_sizeof(uint32_t value_size, uint32_t max_count);

cf_vmapx_err cf_vmapx_create(cf_vmapx* _this, uint32_t value_size, uint32_t max_count, uint32_t hash_size, uint32_t max_name_size);
void cf_vmapx_release(cf_vmapx* _this);

uint32_t cf_vmapx_count(const cf_vmapx* _this);

cf_vmapx_err cf_vmapx_get_by_index(const cf_vmapx* _this, uint32_t index, void** pp_value);
cf_vmapx_err cf_vmapx_get_by_name(const cf_vmapx* _this, const char* name, void** pp_value);

cf_vmapx_err cf_vmapx_get_index(const cf_vmapx* _this, const char* name, uint32_t* p_index);
cf_vmapx_err cf_vmapx_get_index_w_len(const cf_vmapx* _this, const char* name, size_t name_len, uint32_t* p_index);

cf_vmapx_err cf_vmapx_put_unique(cf_vmapx* _this, const void* p_value, uint32_t* p_index);
cf_vmapx_err cf_vmapx_put_unique_w_len(cf_vmapx* _this, const void* p_value, size_t name_len, uint32_t* p_index);


//------------------------------------------------
// Private API - for enterprise separation only.
//

void* cf_vmapx_value_ptr(const cf_vmapx* _this, uint32_t index);

vhash* vhash_create(uint32_t key_size, uint32_t n_rows);
void vhash_destroy(vhash* h);
bool vhash_put(vhash* h, const char* key, size_t key_len, uint32_t value);
