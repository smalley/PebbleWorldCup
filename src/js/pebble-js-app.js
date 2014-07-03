// Function to send a message to the Pebble using AppMessage API
function sendMessage() {
	Pebble.sendAppMessage({"status": 0});
	
	// PRO TIP: If you are sending more than one message, or a complex set of messages, 
	// it is important that you setup an ackHandler and a nackHandler and call 
	// Pebble.sendAppMessage({ /* Message here */ }, ackHandler, nackHandler), which 
	// will designate the ackHandler and nackHandler that will be called upon the Pebble 
	// ack-ing or nack-ing the message you just sent. The specified nackHandler will 
	// also be called if your message send attempt times out.
}

function sendText(scorestring) {
  Pebble.sendAppMessage({"3": scorestring});
}

function sendVibe(vibtype) {
  if(vibtype) {
    console.log("picky eh");
  } else {
    Pebble.sendAppMessage({4: 1});
  }
}

function getMatchData() {
  console.log("Get Match Data")
  var req = new XMLHttpRequest();
  req.open('GET', 'http://worldcup.sfg.io/matches/current', true);
  console.log("Got Data")
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        var response = JSON.parse(req.responseText);
        if(response.length > 0) {
          if(response[0].winner) {
            sendText("WIN: " + response[0].winner);
            sendVibe(null);
          } else {
            console.log("got running match")
            sendText(response[0].home_team.code + "[" + response[0].home_team.goals + "] -- " +
                    response[0].away_team.code + "[" + response[0].away_team.goals + "]");
          }
        } else {
          sendText("No Current Match");
        }
      } else { console.log("Error"); sendText("ERROR!");}
    }
  }
  req.send(null);
}

// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
                getMatchData();
							});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
                //console.log(JSON.stringify(e));
                if(e.payload["2"] == 5) {
                  console.log("Received: Get Score");
                  getMatchData();
                }
							});