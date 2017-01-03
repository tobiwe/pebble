
// Configuration file
var configuration_url = 'http://towan.de/pebble/fhemcontrol/configurable.html';

//Predefined FHEM-Server
var fhem_server = '';
var list_cmd = '?cmd=jsonlist2';
var url =fhem_server+list_cmd;
var fullurl = url+"&XHR=1";

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

//This list contains the devices
var list = [];

//Contains the messages
var messages = [];

//Error state variable
var error_state = false;

//Sending messages (waiting for ready)
function sendNextMessage() {

  if (messages.length === 0) {
    return;
  }

  var message = messages.shift();
  Pebble.sendAppMessage(message, ack, nack);

  function ack() {
    sendNextMessage();
  }

  function nack() {
    messages.unshift(message);
    sendNextMessage();
  }

}

//This function adds an device to the device list
function addDeviceToList(device) {
	//if room exits not generate room

	var roomexists = false;

	for (var i = 0; i < list.length; i++) {
		if (list[i].room === device.room) {
			
			list[i].devices.push({
				name : device.name,
        alias: device.alias,
				state : device.state,
				room : device.room
			});
			roomexists = true;
		}
	}

	if (roomexists === false) {

		list.push({
			room : device.room,
			devices : [{
					name : device.name,
          alias : device.alias,
					state : device.state,
					room : device.room
				}
			]
		});
	}
}

function getData(myUrl) {

    //url = fhem_server + list_cmd;
  
  //url = "http://towan.de/pebble/test/fs20.json";
  //url = "http://towan.de/pebble/test/result.json";
  //url = "http://192.168.0.11:8083/fhem?cmd=jsonlist2&XHR=1";
  
	//Trying to load local stored item
	var server = localStorage.getItem('fhem_server');
	var json = JSON.parse(server);

	if (json === null) {
		console.log("localStorage \'fhem_server\' not found");
	} else {
		console.log("localStorage \'fhem_server\' found: " + json + " (" + json.station + ")");
		fhem_server = json.station;
	}
  
  url = fhem_server+list_cmd;
  fullurl = url+"&XHR=1";
  myUrl=fullurl;
  //myUrl="http://towan.de/pebble/test/all.json";

   xhrRequest(myUrl, 'POST', 
		function (responseText) {
		// Success!
		console.log('jsonlist2 recieved');

		try {
			var json = JSON.parse(responseText);
      var device;
      var devicelist = "";

			for (var x = 0; x < json.Results.length; x++) {
               devicelist = "";
				//Get CUL device
				if (json.Results[x].Internals.TYPE == "IT" && json.Results[x].Attributes.model == "itswitch") {
           device =  {name: json.Results[x].Name,alias: json.Results[x].Name ,state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
           devicelist = device.name + ";" + device.alias + ";"+ device.state + ";" + device.room + ";";
					addDeviceToList(device);
					} else if (json.Results[x].Internals.TYPE == "HUEDevice" && json.Results[x].Attributes.subType == "dimmer") {
					
           device =  {name: json.Results[x].Name,alias: json.Results[x].Attributes.alias, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
           devicelist = device.name + ";" + device.alias + ";"+ device.state + ";" + device.room + ";";
					addDeviceToList(device);
				} else if (json.Results[x].Internals.TYPE == "CUL_HM" && json.Results[x].Attributes.subType == "switch") {
					 device =  {name: json.Results[x].Name,alias: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					   devicelist = device.name + ";" + device.state + ";" + device.room + ";";
          addDeviceToList(device);
          
				}else if (json.Results[x].Internals.TYPE == "FS20" && (json.Results[x].Attributes.model == "fs20fms" || json.Results[x].Attributes.model == "fs20st" )) {
				  device =  {name: json.Results[x].Name, alias: json.Results[x].Name,state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					 devicelist = device.name + ";" + device.alias + ";"+ device.state + ";" + device.room + ";";
          addDeviceToList(device);
      
				}
        else if (json.Results[x].Internals.TYPE == "EnOcean") {
				  device =  {name: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
				 devicelist = device.name + ";" + device.alias + ";"+ device.state + ";" + device.room + ";";
          addDeviceToList(device);
      
				}
      
        if(devicelist!=="")
          {
               // Assemble dictionary using our keys
      //ToDo: Transmit data as an array
    var message = {
      'DEVICE': devicelist
      };
            
            console.log('message: ' + JSON.stringify(message));
            
           messages.push(message);
            
          } 
 
			}
      
      sendNextMessage();
      
		} catch (e) {
			console.log('arsing json failed' + e);
			error_state = true;
		}

	
	});
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
    getData(fullurl);
  });

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',  function(e) {
    console.log('AppMessage received!');
    var result = e.payload.SET.split(",");
    var cmd = "cmd." + result[0] + "=set "+result[0] +" "+ result[1] + "&XHR=1";
  console.log("CMD:" +fhem_server + cmd);
    xhrRequest(fhem_server + cmd, 'POST', 
		function (responseText) {
      //getData(url+ " NAME=" + result[0] +"&XHR=1");
      //console.log("mySpecialMessage: " + url+ " NAME=" + result[0] +"&XHR=1");
    });
  
  
  //ToDo: Replace by request for inly the one data.
  setTimeout(function(){getData(url+ " NAME=" + result[0] +"&XHR=1");}, 1000);
  
  } );

//show configuration event listener
Pebble.addEventListener("showConfiguration", function () {
	console.log("showing configuration");
	Pebble.openURL(configuration_url);
});

//webview closed event listener
Pebble.addEventListener("webviewclosed", function (e) {
	console.log("configuration closed" + e.response);
	var fhem_server = JSON.parse(decodeURIComponent(e.response));
  console.log("New server URL: " + fhem_server);
	localStorage.setItem('fhem_server', JSON.stringify(fhem_server));
	getData(fullurl);
});