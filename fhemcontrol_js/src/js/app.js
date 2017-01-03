/**
 * fhemControl
 * author: Tobi
 * url: pebble.towan.de
 * This app lets you control your fhem devices
 */

//Dependencies
var UI = require('ui');
var ajax = require('ajax');

//Server if not configured
//var fhem_server = '';
var fhem_server = '';

//Configuration url
var configuration_url = 'http://towan.de/pebble/fhemcontrol/configurable.html';

//The command to recieve all information from FHEM
var list_cmd = '?cmd=jsonlist2&XHR=1';

//This list contains the devices
var list = [];

//Error state variable
var error_state = false;

//Structure of State
var state = {
	ON : {
		value : 0,
		name : "on"
	},
	OFF : {
		value : 1,
		name : "off"
	}
};

//Entry point as menue
var main = new UI.Card({
		banner : "images/home_96.png",
		title : "FhemControl",
		body : 'Loading Data...',
		subtitleColor : 'indigo', // Named colors
		bodyColor : '#9a0036' // Hex colors
	});

function getState(s)
{
  if (s=="on")
    {
      return state.ON;
    }
  return state.OFF;
}

//This function adds an device to the device list
function addDeviceToList(device) {
	console.log("arraylength" + list.length);
	console.log("Device_ *: " + device);
	//if room exits not generate room

	var roomexists = false;

	for (var i = 0; i < list.length; i++) {
		if (list[i].room === device.room) {
			console.log("Add device to exitsting room " + device.name + ", " + device.room + ")");
			list[i].devices.push({
				name : device.name,
				state : getState(device.state),
				room : device.room
			});
			roomexists = true;
		}
	}

	if (roomexists === false) {
		console.log("Add device to new room " + device.name + ", " + device.room + ")");
		list.push({
			room : device.room,
			devices : [{
					name : device.name,
					state : getState(device.state),
					room : device.room
				}
			]
		});

		//list[0].room
	}
}

//This function returns the next state
function newState(s) {
	if (s === state.ON) {
		return state.OFF;
	} else {
		return state.ON;
	}
}

//Read Data out of http://192.168.0.27:8083/fhem?cmd=jsonlist&XHR=1
function getData() {
	error_state = false;
	//Trying to load local stored item
	var server = localStorage.getItem('fhem_server');
	var json = JSON.parse(server);

	if (json === null) {
		//direction = "Karlsruhe, Ettlinger Tor";
		console.log("localStorage \'fhem_server\' not found");
	} else {
		console.log("localStorage \'fhem_server\' found: " + json + " (" + json.station + ")");
		fhem_server = json.station;
	}

	main.show();
	ajax({
		url : fhem_server + list_cmd,
		type : 'GET',
	},
		function (data) {
		// Success!
		console.log('Successfully loaded data!');

		try {
			var json = JSON.parse(data);var device;

			for (var x = 0; x < json.Results.length; x++) {
        console.log("x:" +x);
				//Get CUL device
				if (json.Results[x].Internals.TYPE == "IT" && json.Results[x].Attributes.model == "itswitch") {
					console.log("IT-Device found");
           device =  {name: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					addDeviceToList(device);
					} else if (json.Results[x].Internals.TYPE == "HUEDevice") {
					console.log("HUE-Device found");
           device =  {name: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					addDeviceToList(device);
				} else if (json.Results[x].Internals.TYPE == "CUL_HM" && json.Results[x].Attributes.subType == "switch") {
					 device =  {name: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					addDeviceToList(device);
          console.log("CUL_HM switch found");
				}else if (json.Results[x].Internals.TYPE == "FS20" && (json.Results[x].Attributes.model == "fs20fms" || json.Results[x].Attributes.model == "fs20st" )) {
				  device =  {name: json.Results[x].Name, state: json.Results[x].Internals.STATE, room: json.Results[x].Attributes.room};
					addDeviceToList(device);
          console.log("CUL_HM switch found");
				}
          

			}
		} catch (e) {
			console.log('Json failed: ' + e);
			main.body('Check settings!');
			error_state = true;
		}

		if (error_state === false) {
			main.body('Press select');
			main.on('click', 'select', function (e) {

				var menu = new UI.Menu({
						sections : ""
					});

				for (var i = 0; i < list.length; i++) {
					menu.section(i, {
						title : list[i].room
					});

					for (var j = 0; j < list[i].devices.length; j++) {
						menu.item(i, j, {
							title : list[i].devices[j].name + ' [' + list[i].devices[j].state.name + ']',
							subtitle : 'Switch ' + state.ON.name
						});
					}

				}

				menu.show();

				menu.on('select', function (e) {
					console.log('Selected item #' + e.itemIndex + ' of section #' + e.sectionIndex);
					console.log('The item is titled "' + e.item.title + '"');

					list[e.sectionIndex].devices[e.itemIndex].state = newState(list[e.sectionIndex].devices[e.itemIndex].state);

					menu.item(e.sectionIndex, e.itemIndex, {
						title : list[e.sectionIndex].devices[e.itemIndex].name + ' [' + list[e.sectionIndex].devices[e.itemIndex].state.name + ']',
						subtitle : 'Switch ' + newState(list[e.sectionIndex].devices[e.itemIndex].state).name
					});

					console.log("list[e.sectionIndex].device[e.itemIndex].name" + list[e.sectionIndex].devices[e.itemIndex].name);
					var url = fhem_server + "cmd." + list[e.sectionIndex].devices[e.itemIndex].name + "=set%20" + list[e.sectionIndex].devices[e.itemIndex].name + "%20" + list[e.sectionIndex].devices[e.itemIndex].state.name + "&amp;room=" + list[e.sectionIndex].devices[e.itemIndex].room;
					console.log("URL: " + url);
					ajax({
						url : url,
						type : 'GET'
					},
						function (data) {
						// Success!
						console.log('Successfully loaded data!');
					},
						function (error) {
						// Failure!
						console.log('Failed loading data: ' + error);
						error_state = true;
						main.body('Check settings!');
					});

				});

			}, function (error) {
				console.log('Ajax failed: ' + error);
				main.body('Check settings!');
			});
		} else {
			main.body('Check settings!');
		}
	});
}

//This funtciont start the application
function startApplication() {
	getData();
}

//show configuration event listener
Pebble.addEventListener("showConfiguration", function () {
	console.log("showing configuration");
	Pebble.openURL(configuration_url);
});

//webview closed event listener
Pebble.addEventListener("webviewclosed", function (e) {
	console.log("configuration closed" + e.response);
	var fhem_server = JSON.parse(decodeURIComponent(e.response));
	console.log("fhem_server: " + fhem_server);
	localStorage.setItem('fhem_server', JSON.stringify(fhem_server));
	getData();
});

startApplication();