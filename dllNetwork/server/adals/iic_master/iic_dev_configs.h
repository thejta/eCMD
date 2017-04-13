//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG

/****************************** Device Definitions ****************************/

#define ADAL_IIC_XFR_DEFAULT \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	0,\
	/*inc_addr*/	0,\
	/*timeout*/	5000,\
	/*wdelay*/	0,\
	/*rdelay*/	0,\
	/*wsplit*/	0,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 

#define ADAL_IIC_XFR_24C02 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_1BYTE,\
	/*inc_addr*/	0,\
	/*timeout*/	5000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	8,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 

#define ADAL_IIC_XFR_24C04 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_1BYTE,\
	/*inc_addr*/	ADAL_IIC_VAL_1BIT,\
	/*timeout*/	5000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	16,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 
#define ADAL_IIC_XFR_24C08 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_1BYTE,\
	/*inc_addr*/	ADAL_IIC_VAL_2BIT,\
	/*timeout*/	5000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	16,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 
#define ADAL_IIC_XFR_24C16 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_1BYTE,\
	/*inc_addr*/	ADAL_IIC_VAL_3BIT,\
	/*timeout*/	5000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	16,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 
#define ADAL_IIC_XFR_24C64 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_2BYTE,\
	/*inc_addr*/	0,\
	/*timeout*/	10000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	32,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 
#define ADAL_IIC_XFR_24C256 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_2BYTE,\
	/*inc_addr*/	0,\
	/*timeout*/	15000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	64,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 
#define ADAL_IIC_XFR_24C512 \
{\
	/*speed*/	ADAL_IIC_VAL_400KHZ,\
	/*dev_addr*/	0x0,\
	/*dev_width*/	ADAL_IIC_VAL_2BYTE,\
	/*inc_addr*/	0,\
	/*timeout*/	15000,\
	/*wdelay*/	20,\
	/*rdelay*/	0,\
	/*wsplit*/	128,\
	/*rsplit*/	32768,\
	/*offset*/	0,\
	/*flags*/	0,\
} 

/************************** Recovery Policies *********************************/
#define ADAL_IIC_REC_DO_OR_DIE \
{\
	/*redo_pol*/	ADAL_IIC_VAL_ALL_ERRS | 5,\
	/*spd_pol*/	ADAL_IIC_VAL_ALL_ERRS | 1,\
	/*redo_delay*/	200,\
} 

#define ADAL_IIC_REC_MED \
{\
	/*redo_pol*/	ADAL_IIC_VAL_ALL_ERRS | 2,\
	/*spd_pol*/	ADAL_IIC_VAL_ADDR_NOACK | ADAL_IIC_VAL_DATA_NOACK | 1,\
	/*redo_delay*/	20,\
}

#define ADAL_IIC_REC_LITE \
{\
	/*redo_pol*/	ADAL_IIC_VAL_ALL_ERRS | 1,\
	/*spd_pol*/	0,\
	/*redo_delay*/	20,\
} 

#define ADAL_IIC_REC_NONE {0, } 


/**************************** Convenience Macros *****************************/
#define ADAL_IIC_SET_ADDR(d_opts, s_addr) (d_opts)->xfr_opts.dev_addr = s_addr 

#define ADAL_IIC_INIT_XFR(d_opts, s_xfr) \
({\
	adal_iic_xfr_opts_t t_xfr = s_xfr;\
	memcpy(&((d_opts)->xfr_opts), &t_xfr, sizeof(adal_iic_xfr_opts_t));\
})


#define ADAL_IIC_INIT_RECOVERY(d_opts, s_rec) \
({\
	adal_iic_rec_pol_t t_rec = s_rec;\
	memcpy(&((d_opts)->recovery), &t_rec, sizeof(adal_iic_rec_pol_t));\
})

#define ADAL_IIC_RETRY_ERR_MASK              0xFFFF0000
#define ADAL_IIC_RETRY_ERR_COUNT_MASK        0x0000FFFF

/* ADAL_IIC_SET_RECOVERY
 *
 * Specify all recovery details on fail of a IIC master transfer
 *
 * Parameters
 * d_opts: 	The configuration options allocated by the caller
 * retry_errs:	Errors to retry
 * 
 *		This is a bitwise mask of errors, if more than one type is required 
 *		then logical OR in these types.
 *
 *  	   	ADAL_IIC_VAL_ADDR_NOACK		Address NACK error
 *       	ADAL_IIC_VAL_DATA_NOACK 	Data NACK error
 *       	ADAL_IIC_VAL_TIMEOUT		Timeout error
 *       	ADAL_IIC_VAL_LOST_ARB 		Master lost arbitration error
 *       	ADAL_IIC_VAL_BUS_ERR		Low level bus error
 *       	ADAL_IIC_VAL_ALL_ERRS 		All errors
 *			
 * retry_count:	Number of times to retry on error(s)
 * retry_delay:	Time to wait before retry on error in milliseconds
 */
#define ADAL_IIC_SET_RECOVERY(d_opts, retry_errs, retry_count, retry_delay) \
({\
	(d_opts)->recovery.redo_pol = (retry_errs & ADAL_IIC_RETRY_ERR_MASK);\
	(d_opts)->recovery.redo_pol |= (retry_count & ADAL_IIC_RETRY_ERR_COUNT_MASK);\
	(d_opts)->recovery.redo_delay = retry_delay;\
})	
