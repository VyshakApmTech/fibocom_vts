#ifndef __BT_FEATURES_H__
#define __BT_FEATURES_H__

#define IS_ENABLED(var)  (var)



#define SPP_FEATURE_INCLUDE                 TRUE
#define A2DP_FEATURE_INCLUDE                TRUE
#define HFP_FEATURE_INCLUDE                 TRUE
#define AVRCP_FEATURE_INCLUDE               TRUE
#define LE_FEATURE_INCLUDE                  TRUE
#define BR_FEATURE_INCLUDE                  TRUE

#define A2DP_SOURCE_FEATURE_INCLUDE         FALSE
#define HFP_AG_FEATURE_INCLUDE              FALSE
#define HOGP_FEATURE_INCLUDE                FALSE


#define CRPTO_P256_FEATURE_INCLUDE          TRUE


#define HID_FEATURE_INCLUDE                 FALSE


/****************** Features ***************/
#define RDABT_SSP_SUPPORT                   1
#define RDABT_EIR_SUPPORT                   1
#define RDABT_INQUIRY_SUPPORT               1
#define RDABT_AT_PBAP_SUPPORT               0

#define RDABT_OOB_SUPPORT                   0

/* for bt 3.0 */
#define AMP_MANAGER_SUPPORT                 0
#define PAL_IN_HOST                         0

#define RDABT_THREAD_EVENT_SUPPORT          0        // event for call bt profile from other thread

#define RDABT_DHKEY_SUPPORT                 1
#define MEMORY_POOL_SUPPORT                 0        // stack internal memory manager


/******************* Manager ****************/

/****************** L2CAP *******************/
#define L2CAP_CONFIG_VALUES_BELOW_HCI       1
#define L2CAP_FRAGMENT_ACL_SUPPORT          1

#define PTS_TEST_SITUATION_ADAPTATION 0
#define L2CAP_RETRANS_SUPPORT         0

/*************** Test and Debug *************/
/* Debug option */
#define NO_SEC_CHECK

#endif

