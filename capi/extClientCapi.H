/**
 * @mainpage

@section ext eCMD Extensions

This is the documentation of the eCMD C/C++ Programming API extensions

\if ECMD_CMD_EXTENSION_SUPPORT
@subsection cmd CMD Command line Extension
This extension provides interfaces to call command line functions and have formatted data displayed to stdout or returned to the caller.  It supports command from the core command line and also all extensions.<br>

Include files :
<ul>
<li> cmdClientCapi.H</li>
<li> cmdStructs.H</li>
<li> Library : cmdClientCapi.a</li>
</ul>
\endif

\if ECMD_CIP_EXTENSION_SUPPORT
@subsection cip CIP (Cronus/IP) Extension
This extension provides interfaces to start/stop processor instructions and breakpoint handling.<br>

Include files :
<ul>
<li> cipClientCapi.H</li>
<li> cipStructs.H</li>
<li> Library : cipClientCapi.a</li>
</ul>
\endif

\if ECMD_GIP_EXTENSION_SUPPORT
@subsection gip GIP GFW IP-Series Extension
This extension provides IP Series GFW only interfaces.<br>

Include files :
<ul>
<li> gipClientCapi.H</li>
<li> gipStructs.H</li>
<li> Library : gipClientCapi.a</li>
</ul>
\endif

\if ECMD_CRO_EXTENSION_SUPPORT
@subsection cronus Cronus Extension
This extension provides Cronus only interfaces.<br>

Include files :
<ul>
<li> croClientCapi.H</li>
<li> croStructs.H</li>
<li> Library : croClientCapi.a</li>
</ul>
\endif

\if ECMD_ZSE_EXTENSION_SUPPORT
@subsection zse Z Series Extension
This extension provides Z-Series only interfaces.<br>

Include files :
<ul>
<li> zseClientCapi.H</li>
<li> zseStructs.H</li>
<li> Library : zseClientCapi.a</li>
</ul>
\endif

\if ECMD_EIP_EXTENSION_SUPPORT
@subsection eip EIP (Eclipz I/P) Extension
This extension provides Eclipz only interfaces.<br>

Include files :
<ul>
<li> eipClientCapi.H</li>
<li> eipStructs.H</li>
<li> Library : eipClientCapi.a</li>
</ul>
\endif

\if ECMD_AIP_EXTENSION_SUPPORT
@subsection aip AIP (Apollo I/P) Extension
This extension provides Apollo only interfaces.<br>

Include files :
<ul>
<li> aipClientCapi.H</li>
<li> aipStructs.H</li>
<li> Library : aipClientCapi.a</li>
</ul>
\endif

\if ECMD_SCAND_EXTENSION_SUPPORT
@subsection scand ScanD Extension
This extension provides ScanD only interfaces.<br>

Include files :
<ul>
<li> scandClientCapi.H</li>
<li> scandStructs.H</li>
<li> Library : scandClientCapi.a</li>
</ul>
\endif

\if ECMD_BML_EXTENSION_SUPPORT
@subsection bml BML Extension
This extension provides BML only interfaces and is only available running eCMD in a BML environment.<br>

Include files :
<ul>
<li> bmlClientCapi.H</li>
<li> bmlStructs.H</li>
<li> Library : bmlClientCapi_ppc.a </li>
</ul>
\endif

\if ECMD_MBO_EXTENSION_SUPPORT
@subsection mbo MBO (Mambo) Extension
This extension provides Mambo Simulator only interfaces.<br>

Include files :
<ul>
<li> mboClientCapi.H</li>
<li> mboStructs.H</li>
<li> Library : mboClientCapi.a</li>
</ul>
\endif

\if ECMD_FAPI_EXTENSION_SUPPORT
@subsection fapi FAPI (Hardware Procedure Framework) Extension
This extension provides HWPF fapi only interfaces.  
<ul>
<li> eCMD specific headers</li>
  <p><b>NOTE: These are needed in eCMD procedures ONLY not actual fapi procedures that will be consumed by FW</b></p>
    <ul>
    <li> fapiClientCapi.H</li>
    <li> fapiStructs.H</li>
    <li> fapiSharedUtils.H</li>
    </ul>

<li>Hardware Procedure Framework (HWPF) specific headers</li>
  <p><b>NOTE: fapi.H includes all the headers below.  You should include the fapi.H header in a fapi Hardware Procedure (HWP)</b></p>
    <ul>
    <li> fapi.H </li>
       <ul>
       <li>fapiTarget.H </li> 
       <li>fapiReturnCode.H</li>
       <li>fapiUtil.H</li>
       <li>fapiHwAccess.H</li>
       <li>fapiSystemConfig.H</li>
       <li>fapiPlatTrace.H</li>
       <li>fapiHwpExecutor.H</li>
       <li>fapiAttributeService.H</li>
       <li>fapiChipEcFeature.H</li>
       <li>fapiHwpReturnCodes.H</li>
       <li>fapiHwpErrorInfo.H</li>
       <li>fapiAttributeIds.H</li>
       <li>fapiSetSbeError.H</li>
       <li>fapiMvpdAccess.H</li>
       <li>fapiMBvpdAccess.H</li>
       <li>ecmdDataBufferBase.H</li>
       </ul>
    </ul>
<li> Library : fapiClientCapi.a</li>
<li> Library : libfapi.so</li>
</ul>
\endif

\if ECMD_FAPI2_EXTENSION_SUPPORT
@subsection fapi2 FAPI2 (Hardware Procedure Framework) Extension
This extension provides HWPF fapi2 only interfaces.  
<ul>
<li> eCMD specific headers</li>
  <p><b>NOTE: These are needed in eCMD procedures ONLY not actual fapi procedures that will be consumed by FW</b></p>
    <ul>
    <li> fapi2ClientCapi.H</li>
    <li> fapi2SharedUtils.H</li>
    </ul>

<li>Hardware Procedure Framework (HWPF) specific headers</li>
  <p><b>NOTE: fapi2.H includes all the headers below.  You should include the fapi2.H header in a fapi2 Hardware Procedure (HWP)</b></p>
    <ul>
    <li> fapi2.H </li>
       <ul>
       <li>target.H </li> 
       <li>return_code.H</li>
       <li>buffer.H</li>
       <li>hw_access.H</li>
       <li>utils.H</li>
       <li>plat_trace.H</li>
       <li>error_scope.H</li>
       <li>set_sbe_error.H</li>
       <li>fapi2_hwp_executor.H</li>
       <li>fapi2_attribute_service.H</li>
       <li>fapi2_chip_ec_feature.H</li>
       <li>mvpd_access.H</li>
       <li>mbvpdAccess.H</li>
       </ul>
    </ul>
<li> Library : fapi2ClientCapi.a</li>
<li> Library : libfapi2.so</li>
</ul>
\endif
