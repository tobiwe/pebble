var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  
  //xhr.open("utf-8",url);
  xhr.open(type, url);
  xhr.send();
};

function getData()
{
  //get station from local storage
  var destination = localStorage.getItem('station');

  var test = JSON.parse(destination);
  var direction ="";
  
  if(test === null)
    {
      //direction = "Karlsruhe, Ettlinger Tor";
    }
  else
  {
    direction = test.station;
  }
  
  console.log("STATION: "+ direction);

  var url ='http://www.efa-bw.de/nvbw/XSLT_DM_REQUEST?outputFormat=JSON&limit=5&language=en&name_dm=' + direction  +'&type_dm=stop&mode=direct&dmLineSelectionAll=1&depType=STOPEVENTS&includeCompleteStopSeq=1&useRealtime=1&itdLPxx_hideNavigationBar=false&itdLPxx_transpCompany=Refresh&timeOffset=0';
  console.log("URL " + url);

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var stationName = json.departureList[0].stopName.substring(0,32);
      
    var linenumber = [];
    var type= [];
    var linename= [];
    var countdown= [];
    var time= [];
   var realtime=[];

      for (var i = 0; i < 5; i++) {
       linenumber[i]  =  json.departureList[i].servingLine.number.substring(0,4);
        if(linenumber[i].length === 0)
          {
             linenumber[i] = ' ';
          }
       type[i] = json.departureList[i].servingLine.motType;
       linename[i] = json.departureList[i].servingLine.direction.substring(0,18);
       countdown[i] = json.departureList[i].countdown;
        
        if(json.departureList[i].dateTime.hour<10)
          {
            json.departureList[i].dateTime.hour = "0" + json.departureList[i].dateTime.hour;
          }
        if(json.departureList[i].dateTime.minute<10)
          {
            json.departureList[i].dateTime.minute = "0" + json.departureList[i].dateTime.minute;
          }
        
        
          
       time[i] = json.departureList[i].dateTime.hour + ':' + json.departureList[i].dateTime.minute;
        
        if(typeof json.departureList[i].realDateTime =='undefined')
          {
                realtime[i] = time[i];
              console.log("readDateTime undefined");
          }
        else
          {
          if(json.departureList[i].realDateTime.hour<10)
            {
              json.departureList[i].realDateTime.hour = "0" + json.departureList[i].realDateTime.hour;
            }
          if(json.departureList[i].realDateTime.minute<10)
            {
              json.departureList[i].realDateTime.minute = "0" + json.departureList[i].realDateTime.minute;
            }
                realtime[i] = json.departureList[i].realDateTime.hour + ':' + json.departureList[i].realDateTime.minute;
          }
        
          
      }               
      
      //console.log('Timetable are ' + unetimetable);
      //console.log('Station is ' + station);
    
      // Assemble dictionary using our keys
      //ToDo: Transmit data as an array
    var dictionary = {
      'KEY_STATION': stationName,
      'KEY_LINE1': type[0] + ';' +linenumber[0] + ';' + linename[0].substring(0,16) + ';' + countdown[0] + ";" + time[0] + ";" + realtime[0],
      'KEY_LINE2': type[1] + ';' +linenumber[1] + ';' + linename[1].substring(0,16) + ';' + countdown[1] + ";" + time[1] + ";" + realtime[1],
      'KEY_LINE3': type[2] + ';' +linenumber[2] + ';' + linename[2].substring(0,16) + ';' + countdown[2] + ";" + time[2] + ";" + realtime[2],
      'KEY_LINE4': type[3] + ';' +linenumber[3] + ';' + linename[3].substring(0,16) + ';' + countdown[3] + ";" + time[3] + ";" + realtime[3],
      'KEY_LINE5': type[4] + ';' +linenumber[4] + ';' + linename[4].substring(0,16) + ';' + countdown[4] + ";" + time[4] + ";" + realtime[4]
      };

    // Send to Pebble
        console.log(dictionary["KEY_STATION"]);
        console.log(dictionary["KEY_LINE1"]);
        console.log(dictionary["KEY_LINE2"]);
        console.log(dictionary["KEY_LINE3"]);
        console.log(dictionary["KEY_LINE4"]);
        console.log(dictionary["KEY_LINE5"]);
      console.log("Fin");
      
    Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log('Table info sent to Pebble successfully!');
    },
    function(e) {
      console.log('Error sending table info to Pebble!');
  }
);
    
    }      
  );
  
}

function locationSuccess(pos) {
}


function locationError(err) {
  console.log('Error requesting location!');
}

function getTransit() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    
    //Get the initial transit
  //getTransit();
    getData();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    
    //Get transit
    //getTransit();
    getData();
  }                     
);

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://towan.de/pebble/abfahrtsmonitor/configurable.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var destination = JSON.parse(decodeURIComponent(e.response));
   console.log("dest = " + destination.station);
   localStorage.setItem('station',JSON.stringify(destination));
  console.log("Options = " + JSON.stringify(destination));
   getData();
});