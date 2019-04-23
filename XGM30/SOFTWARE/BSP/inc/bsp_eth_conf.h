/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM322xG Evaluation Board
*
* Filename      : bsp_eth_conf.h
* Version       : V2.00
* Programmer(s) : Michael Vysotsky
*********************************************************************************************************
*/
#ifndef _BSP_ETH_CONF_H
#define _BSP_ETH_CONF_H

/* Ethernet pins configuration ************************************************/
   /*
        ETH_MDIO -------------------------> PA2
        ETH_MDC --------------------------> PC1
        ETH_PPS_OUT ----------------------> PB5
        ETH_MII_CRS ----------------------> PH2
        ETH_MII_COL ----------------------> PH3
        ETH_MII_RX_ER --------------------> PI10
        ETH_MII_RXD2 ---------------------> PH6
        ETH_MII_RXD3 ---------------------> PH7
        ETH_MII_TX_CLK -------------------> PC3
        ETH_MII_TXD2 ---------------------> PC2
        ETH_MII_TXD3 ---------------------> PB8
        ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
        ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
        ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
        ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
        ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
        ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
        ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
                                                  */
#ifdef MII_MODE  //ETH_RX_CLK_PORT
#define ETH_MDIO_PORTA
#define ETH_MDC_PORTC
#define ETH_PPS_OUT_PORTB
#define ETH_MII_CRS_PORTA
#define ETH_MII_COL_PORTA
#define ETH_MII_RX_ER_PORTB
#define ETH_MII_RXD2_PORTB
#define ETH_MII_RXD3_PORTB
#define ETH_MII_TX_CLK_PORTC
#define ETH_MII_TXD2_PORTC
#define ETH_MII_TXD3_PORTB
#define ETH_MII_RX_CLK_PORTA
#define ETH_MII_RX_DV_PORTA
#define ETH_MII_RXD0_PORTC
#define ETH_MII_RXD1_PORTC
#define ETH_MII_TX_EN_PORTB
#define ETH_MII_TXD0_PORTB
#define ETH_MII_TXD1_PORTB
#endif



/** MDIO------------------------------>PA2
    MDC ------------------------------>PC1
    RXD0------------------------------>PC4
    RXD1------------------------------>PC5
    CRS_DV------------------------------>PA7
    TX_EN ------------------------------>PB11
    TXD0  ------------------------------>PB12
    TXD1  ------------------------------>PB13
    REF_CLK ---------------------------->PA1**/

#ifdef RMII_MODE

#define ETH_RMII_REF_CLK_PORTA    //--PA1
#define ETH_MDIO_PORTA            //--PA2
#define ETH_RMII_CRS_DV_PORTA     //--PA7

#define ETH_RMII_TX_EN_PORTB      //--PB11
#define ETH_RMII_TXD0_PORTB       //--PB12
#define ETH_RMII_TXD1_PORTB       //--PB13

#define ETH_MDC_PORTC             //--PC1
#define ETH_RMII_RXD0_PORTC       //--PC4
#define ETH_RMII_RXD1_PORTC       //--PC5
#endif

//#define PHY_MICR                  0x11 /* MII Interrupt Control Register */
//#define PHY_MICR_INT_EN           ((uint16_t)0x0002) /* PHY Enable interrupts */
//#define PHY_MICR_INT_OE           ((uint16_t)0x0001) /* PHY Enable output interrupt events */
//#define PHY_MISR                    0x12 /* MII Interrupt Status and Misc. Control Register */
//#define PHY_MISR_LINK_INT_EN      ((uint16_t)0x0020) /* Enable Interrupt on change of link status */
#define PHY_LINK_INTERRUPT         ((uint16_t)0x2000) /* PHY link status interrupt mask */
#define PHY_LINK_STATUS           ((uint16_t)0x0004)  /*!< PHY Link mask                                   */
#endif