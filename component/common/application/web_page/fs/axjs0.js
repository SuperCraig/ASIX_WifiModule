var confirmType='';
function ChkNumberRange(inValue, minValue, maxValue){
	if (inValue=='' || isNaN(inValue) || (inValue < minValue) || (inValue > maxValue)) return false;
	else return true;
}
function ChkIsHexCode(inValue, inLen){
	var i;
	for (i = 0; i < inLen; i ++){
		if (!(inValue.charAt(i) >= '0' && inValue.charAt(i) <= '9' || 
			  inValue.charAt(i) >= 'a' && inValue.charAt(i) <= 'f' || 
			  inValue.charAt(i) >= 'A' && inValue.charAt(i) <= 'F'))
			return false;
	}
	return true;
}
function ChkIp(s){
	var i;
	var split_s = s.split('.');
	var dotCnt = split_s.length;
	if (dotCnt != 4) return -1;
	for (i=0; i<dotCnt ; i++){
		if ((split_s[i].length > 3) || (split_s[i].length == 0)) return -1;
		if (isNaN(split_s[i]) || split_s[i] > 255 || split_s[i] < 0) return -1;
	}
	return split_s;
}
function GetSubnet(ip_str, mask_str){
	var ip = ChkIp(ip_str);
	var mask = ChkIp(mask_str);
	var subnet=["","","",""], i;
	for (i=0; i<4; i++)
		subnet[i] = Number(ip[i]) & Number(mask[i]);
	return (subnet[0]+"."+subnet[1]+"."+subnet[2]+"."+subnet[3]);
}
function ChkIpFormat(s){
	var i, j=0;
	for (i=0; i<s.length; i++){
		if (s.charAt(i) != '.' && isNaN(s.charAt(i)))
			return false;
	}
	return true;
}
function ChkEmail(s){
	var pattern = /^[\w\.\-]+@([\w\-]+\.)+[a-zA-Z]+$/;
	return pattern.test(s);
}
function chkForm(objform) {
	var items = objform.elements, i, errFlag = 0, groupErrFlag = 0, typetmp, ip, def_subnet=0, net_mask=0, wifi_mode=0;
	for (i = 0; i < items.length; i ++) {
		var errMsg = "";
		if (confirmType=='reboot' || confirmType=='restore') break;
		switch (items[i].name) {
		case "aes_passphrase":
			if (ChkNumberRange(items[i].value.length, 8, 63) == false){errMsg = " Invalid";}
			break;
		case "txtimer":
		case "smtp_port":
			if (ChkNumberRange(items[i].value, 10, 65535) == false){errMsg = " Invalid";}
			break;
		case "s_lstport":
		case "c_desport":
			if (ChkNumberRange(items[i].value, 1024, 65535) == false){errMsg = " Invalid";}
			break;
		case "mb_rspTime":
			if (ChkNumberRange(items[i].value, 10, 65000) == false){errMsg = " Invalid";}
			break;
		case "mb_frameIntv":
		case "mb_CharDelay":
			if (ChkNumberRange(items[i].value, 10, 500) == false){errMsg = " Invalid";}
			break;
		case "static_ip":
		case "gateway_ip":
		case "wz_static_ip":
		case "wz_gateway_ip":
			ip = ChkIp(items[i].value);
			if (ip == -1 || ip[3]==255 || ip[3]==0 || (ip[0]&0xF0)>=0xE0 || ((ip[0]&0xC0) && ip[2]==0xFF)){errMsg = " Invalid";}
			if (items[i].name=="static_ip" || items[i].name=="wz_static_ip"){
				if (GetSubnet(items[i].value, items[i+2].value) != GetSubnet(items[i+1].value, items[i+2].value)){errMsg = " Invalid subnet with default gateway";}
				else {def_subnet = GetSubnet(items[i].value, items[i+2].value); net_mask=items[i+2].value;}
			}
			break;
		case "dhcps_startip":
		case "dhcps_endip":
			ip = ChkIp(items[i].value);
			if (ip == -1 || ip[3]==255 || ip[3]==0 || (ip[0]&0xF0)>=0xE0 || ((ip[0]&0xC0) && ip[2]==0xFF)){errMsg = " Invalid";}
			if (def_subnet){if (GetSubnet(items[i].value, net_mask) != def_subnet){errMsg = " Invalid subnet with default gateway";}}
			if (items[i].name=="dhcps_endip"){
				var k;
				var staip = ChkIp(items[i-1].value);
				for (k=0;k<4;k++){
					if ((net_mask[k]&staip[k]) != (net_mask[k]&ip[k])){break;}
				}
				if (k<4){errMsg = " Invalid IP pool (net ID error)";break;}
				for (k=0;k<4;k++){
					if ((~net_mask[k]&staip[k]) < (~net_mask[k]&ip[k])){break;}
				}
				if (k==4){errMsg = " Invalid IP pool (end pool must be larger than start pool)";break;}
			}
			break;
		case "dns_ip":
		case "wz_dns_ip":
			ip = ChkIp(items[i].value);
			if (ip == -1 || ip[3]==255 || ip[3]==0 || (ip[0]&0xF0)>=0xE0 || ((ip[0]&0xC0) && ip[2]==0xFF)){errMsg = " Invalid";}
			break;
		case "mask":
		case "wz_mask":
			ip = ChkIp(items[i].value);
			if (ip == -1 || ip[0]==0 || ip[1]==0){errMsg = " Invalid";}
			break;
		case "smtp_un":
		case "smtp_pw":
		case "smtp_cfm_pw":
		case "old_psw":
		case "new_psw":
		case "cfm_psw":
		case "new_usn":
		case "username":
		case "password":
		case "ssid":
		case "dsm_name":
		case "img_path":
		case "gn_host":
		case "bm_host":
		case "bm_un":
		case "bm_pw":
		case "mbtcp_port":
		case "az_host":
		case "az_hn":
		case "az_di":
		case "az_sk":
		case "az_et":
			
			if (items[i].value == ""){errMsg = " Invalid";}
			break;
		case "key_index_0":
		case "key_index_1":
		case "key_index_2":
		case "key_index_3":
			if (document.getElementById("key_length").value == 0){
				if ((items[i].value.length!=5)&&(items[i].value.length!=10)) {errMsg = " Invalid";}
			}
			else {
				if ((items[i].value.length!=13)&&(items[i].value.length!=26)) {errMsg = " Invalid";}
			}
			if ((items[i].value.length==10)||(items[i].value.length==26))
			{
				if (ChkIsHexCode(items[i].value, items[i].value.length) == false) {errMsg = " Invalid";}
			}
			break;
		case "ntps_name1":
		case "ntps_name2":
		case "ntps_name3":
		case "c_deshn":
		case "email_addr":
			if (items[i].value == "") {errMsg = " Invalid";}
			else if (ChkIpFormat(items[i].value) == true){
				ip = ChkIp(items[i].value);
				if (ip == -1 || ip[3]==255 || ip[3]==0 || (ip[0]&0xF0)>=0xE0 || ((ip[0]&0xC0) && ip[2]==0xFF)){errMsg = " Invalid";}
			}
			break;
		case "email_from":
		case "email_to1":
		case "email_to2":
		case "email_to3":
			if (items[i].value != "" && ChkEmail(items[i].value)==false) {errMsg = " Invalid";}
			break;
		case "rtc_dd":
			if (items[i-2].value == "" || ChkNumberRange(items[i-2].value, 2000, 2099) == false){errMsg = " Invalid year";}
			else if (items[i-1].value == "" || ChkNumberRange(items[i-1].value, 1, 12) == false){errMsg = " Invalid month";}
			else if (items[i].value == "" || ChkNumberRange(items[i].value, 1, 31) == false){errMsg = " Invalid day";}
			else{
				var rtcYear=items[i-2].value;
				var rtcMonth=items[i-1].value;
				var rtcDate=items[i].value;
				rtcYear=rtcYear%4;
				if (rtcMonth==2){
					if (!(rtcYear) && rtcDate>29){errMsg = " The day must be less than 29";}
					else if (rtcYear && rtcDate>28){errMsg = " The day must be less than 28";}
				}else if (rtcMonth<8){
					if (!(rtcMonth&1) && rtcDate>30){errMsg = " The day must be less than 30";}
				}else if (rtcMonth>7 && (rtcMonth&1) && rtcDate>30){errMsg = " The day must be less than 30";}
			}
			break;
		case "rtc_ss":
			if (items[i-2].value == "" || ChkNumberRange(items[i-2].value, 0, 23) == false){errMsg = " Invalid hour";}
			else if (items[i-1].value == "" || ChkNumberRange(items[i-1].value, 0, 59) == false){errMsg = " Invalid minute";}
			else if (items[i].value == "" || ChkNumberRange(items[i].value, 0, 59) == false){errMsg = " Invalid second";}
			break;
		case "network_mode":
			wifi_mode = items[i].value;
			break;
		case "security_mode":
			if (wifi_mode==1)
			{
				if (items[i].value==1)	{errMsg = " Unsupported security type in AP mode";}
			}
			break;
		case "channel":
			if (wifi_mode==1)
			{
				if (ChkNumberRange(items[i].value, minChNum-1, maxChNum-1) == false)	{errMsg = " The valid channel plan is "+minChNum+"~"+maxChNum;}
			}
			break;
		case "bm_orgid":
			if (items[i].value.length!=6) {errMsg = " Invalid";}
			break;
		case "bm_typid":
			if (items[i].value.length!=5) {errMsg = " Invalid";}
			break;
		case "bm_devid":
			if (items[i].value.length!=11) {errMsg = " Invalid";}
			break;
		case "wz_key":
			var enc_type = document.getElementById("enc_type");
			if (enc_type!=null && enc_type.value!='wep')
			{
				if ((items[i].value.length!=0) && ChkNumberRange(items[i].value.length, 8, 63) == false){errMsg = " Invalid";}
			}else {
				if ((items[i].value.length!=0)&&(items[i].value.length!=5)&&(items[i].value.length!=10)&&(items[i].value.length!=13)&&(items[i].value.length!=26)) {errMsg = " Invalid";}
			}
			break;
		default:
			continue;	
		break;
	}		
		var errSpan = document.createElement("span");
		errSpan.appendChild(document.createTextNode(errMsg));
		errSpan.className = "errMsg0";
		errParent = items[i].parentNode;
		if (errParent.lastChild.className == "errMsg0")
			errParent.replaceChild(errSpan, errParent.lastChild);
		else
			errParent.appendChild(errSpan);
		if (errMsg != "") errFlag = 1;
		
}
	if (errFlag) return false;
	typetmp=confirmType;
	confirmType='';
	switch (typetmp){
	case 'submit':
		if (!confirm("To submit configuration! continue?")) return false; break;
	case 'upgrade':
		if (!confirm("The device will perform firmware update, press ok to continue, otherwise press cancel to close this window.")) return false;break;
	case 'submitAndReboot':
		if (!confirm("To submit configuration and reboot device! continue?")) return false;break;
	case 'reboot':
		if (!confirm("To reboot device! continue?")) return false;break;
	case 'restore':
		if (!confirm("To restore device! continue?")) return false;break;
	case 'changeUsername':
		if (!confirm("Change username and login again?")) return false;break;
	case 'changePassword':
		if (!confirm("Change password and login again?")) return false;break;
	}	
	return true;
}

