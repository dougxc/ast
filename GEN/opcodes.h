/*
 * Copyright (C) 1997, Doug Simon
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */


// ### BEGIN GENERATED CODE ###

// Define an enumerated type for the opcodes
enum iType {
	iAdd, iAddcc, iAddx, iAddxcc, iAnd, iAndcc, iAndn, iAndncc, iB,
	iB_a, iBa, iBa_a, iBcc, iBcc_a, iBclr, iBcs, iBcs_a, iBe, iBe_a,
	iBg, iBg_a, iBge, iBge_a, iBgeu, iBgeu_a, iBgu, iBgu_a, iBl,
	iBl_a, iBle, iBle_a, iBleu, iBleu_a, iBlu, iBlu_a, iBn, iBn_a,
	iBne, iBne_a, iBneg, iBneg_a, iBpos, iBpos_a, iBset, iBtog, iBtst,
	iBvc, iBvc_a, iBvs, iBvs_a, iCall, iCb0, iCb0_a, iCb01, iCb01_a,
	iCb012, iCb012_a, iCb013, iCb013_a, iCb02, iCb02_a, iCb023,
	iCb023_a, iCb03, iCb03_a, iCb1, iCb1_a, iCb12, iCb12_a, iCb123,
	iCb123_a, iCb13, iCb13_a, iCb2, iCb2_a, iCb23, iCb23_a, iCb3,
	iCb3_a, iCba, iCba_a, iCbn, iCbn_a, iClr, iClrb, iClrh, iCmp,
	iCpop1, iCpop2, iDec, iDeccc, iFabss, iFaddd, iFaddq, iFadds,
	iFba, iFba_a, iFbe, iFbe_a, iFbg, iFbg_a, iFbge, iFbge_a, iFbl,
	iFbl_a, iFble, iFble_a, iFblg, iFblg_a, iFbn, iFbn_a, iFbne,
	iFbne_a, iFbo, iFbo_a, iFbu, iFbu_a, iFbue, iFbue_a, iFbug,
	iFbug_a, iFbuge, iFbuge_a, iFbul, iFbul_a, iFbule, iFbule_a,
	iFcmpd, iFcmped, iFcmpeq, iFcmpes, iFcmpq, iFcmps, iFdivd, iFdivq,
	iFdivs, iFdmulq, iFdtoi, iFdtoq, iFdtos, iFitod, iFitoq, iFitos,
	iFlush, iFmovs, iFmuld, iFmulq, iFmuls, iFnegs, iFpop1, iFpop2,
	iFqtod, iFqtoi, iFqtos, iFsmuld, iFsqrtd, iFsqrtq, iFsqrts,
	iFstod, iFstoi, iFstoq, iFsubd, iFsubq, iFsubs, iInc, iInccc,
	iJmp, iJmpl, iLd, iLdc, iLdcsr, iLdd, iLddc, iLddf, iLdf, iLdfsr,
	iLdsb, iLdsh, iLdstub, iLdub, iLduh, iMov, iMulscc, iNeg, iNop,
	iNot, iOr, iOrcc, iOrn, iOrncc, iRd, iRestore, iRet, iRetl, iSave,
	iSdiv, iSdivcc, iSet, iSethi, iSll, iSmul, iSmulcc, iSra, iSrl,
	iSt, iStb, iStbar, iStc, iStcsr, iStd, iStdc, iStdcq, iStdf,
	iStf, iStfsr, iSth, iSub, iSubcc, iSubx, iSubxcc, iSwap, iTaddcc,
	iTaddcctv, iTst, iTsubcc, iTsubcctv, iUdiv, iUdivcc, iUmul,
	iUmulcc, iUnimp, iWr, iXnor, iXnorcc, iXor, iXorcc, iInvalid
};

// Convert from iType to a string representation
char const* Type2String(iType t);

// Convert from a string to the matching iType
iType String2Type(const char* opString);

// ### END GENERATED CODE ###

