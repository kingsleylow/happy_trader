<%@ page contentType="text/html;charset=windows-1251"%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<script type="text/javascript" src="http://www.finam.ru/scripts/export.js"></script>
		<script>
			function on_load()
			{
				mForm.target="upload_target";
				mForm.action="/HtProxy.jsp";
				mForm.url_val.value = "http://www.finam.ru/analysis/export/default.asp";

				mForm.submit();
			}

			function on_frame_load()
			{
				var body = frames['upload_target'].document.getElementsByTagName("body")[0];


				if (body.innerHTML.length >0) {

					var marketArray = new Array();
					for(i = 0; i < upload_target.chartform.market.length; i++) {
						marketArray[upload_target.chartform.market.options.item(i).value] = upload_target.chartform.market.options.item(i).innerText;
						
					}

					

					// make the second part
					txt = "/* GENERATED AUTOMATICALLY */\n";
					txt += "public class FinamDownloadedConstants {\n"
					txt += "	//EMITENT_ID,EMITENT_NAME,EMITENT_CODE,EMITENT_MARKET_ID,EMITENT_MARKET\n";
					txt += "  public static Map<String, HtPair<Integer, Integer> > r = new HashMap<String, HtPair<Integer, Integer> >();\n";
					txt += "	public static void init_0() {\n";
				
					var cnt = 0;
					var idx = 1;
					for(i=0; i < aEmitentIds.length; i++) {

						var e_code = aEmitentCodes[i].replace(/\"/g,'\\"');
						var e_name = aEmitentNames[i].replace(/\"/g,'\\"');
						var e_market_name = marketArray[ aEmitentMarkets[i] ];
						var e_id = aEmitentIds[i];
						var e_market_id = aEmitentMarkets[i];

						var combined_symbol = e_market_name+'-'+e_code;

						txt += ' r.put("'+combined_symbol+'", new HtPair<Integer,Integer>('+e_market_id+','+e_id+'));\n';

						if (++cnt >=1400) {
							txt +="		};\n";
							txt += "	public static void init_"+idx+"() {\n";
							cnt = 0;
							idx++;
						}
					}
				
					txt +="		};\n";
					txt +="}\n";
					textAreaObj.innerText = txt;

					

				}

			}


		</script>
	</head>
	<body onload="on_load();">
		<textarea wrap="off" style="overflow:auto;width:100%;height:100%;" ID="textAreaObj"></textarea>
		

		<iframe id="upload_target" name="upload_target" style="display:none" onload="on_frame_load()" src=""></iframe>

		<form method="get" action="" name="mForm" id="mForm" >
			<input type=hidden name="url_val" value=""></input>
		</form>

	</body>
</html>