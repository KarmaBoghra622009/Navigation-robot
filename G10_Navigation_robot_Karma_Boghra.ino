#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Your WiFi credentials
const char* ssid = "Navigation Robot";
const char* password = "Karma123";

// Create an instance of the web server
ESP8266WebServer server(80);

// Motor control pins
#define ENA 0
#define IN1 8
#define IN2 7
#define IN3 6
#define IN4 5
#define ENB 1

// Define the speed of the motors
#define MOTOR_SPEED 150

// HTML content for the main page
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Navigation Robot</title>
    <style>
        body {
            font-family: Calibri, calibri;
        }
        .item-button {
            display: inline-block;
            margin: 10px;
            cursor: pointer;
            border: 2px solid #ddd;
            width: 110px;
            padding: 10px;
            border-radius: 5px;
            transition: all 0.3s ease;
        }
        .item-button:hover {
            border-color: #888;
        }
        .item-button img {
            width: 100px;
            height: 110px;
        }
        .item-container {
            display: flex;
            width: 300px;
            justify-content: space-around;
            align-items: center;
            margin-top: 20px;
        }
        .item-details {
            margin-top: 20px;
        }
        .receipt {
            margin-top: 20px;
        }
        .receipt-table {
            width: 100%;
            border-collapse: collapse;
        }
        .receipt-table th, .receipt-table td {
            border: 1px solid #ddd;
            padding: 8px;
        }
        .receipt-table th {
            background-color: #f2f2f2;
        }
        .quantity-input {
            width: 50px;
            text-align: center;
        }
        #intro {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #ffffff;
        }
        #intro img {
            max-width: 100%;
            max-height: 100%;
        }
        #content {
            display: none;
        }
    </style>
</head>
<body>
    <div id="intro" onclick="startProgram()">
        <img source src="https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExajg3enk0YzAzZnNxbHBpaDN1dHM1Yml3MTIzYnM3djliY2l5OHRiciZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aYUYWi7aik9KrMwnFG/giphy.gif" alt="Welcome GIF">
    </div>
    <div id="content">
        <h1>Welcome to the Navigation Robot Interface</h1>
        <!-- Main page with Chips and Drinks buttons -->
        <div id="main-page">
            <h2>Select an Item</h2>
            <div class="item-container">
                <div class="item-button" onclick="navigate('chips')">
                    <img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ1g03mexKvLHz3Ra4q39GW3LRij0E6IiuHag&s" alt="Chips">
                    <p>Chips</p>
                </div>
                <div class="item-button" onclick="navigate('drinks')">
                    <img src="https://tiimg.tistatic.com/fp/1/008/042/1-25-liter-0-alcohol-content-coca-cola-cold-drink--488.jpg" alt="Drinks">
                    <p>Drinks</p>
                </div>
            </div>
        </div>

        <!-- Item details page -->
        <div id="item-page" style="display:none;">
            <h2 id="item-name"></h2>
            <div class="item-details">
                <p id="item-description"></p>
                <p id="item-price"></p>
            </div>
            <div class="item-container">
                <div class="item-button" id="counter-button" onclick="navigate('counter')">
                    <img src="https://th.bing.com/th/id/R.5d4d859e01eda92728c27753aacda1ec?rik=Rj3ncTT%2bkRy5yQ&riu=http%3a%2f%2fgetdrawings.com%2ffree-icon%2fretail-store-icon-65.png&ehk=%2f%2fAQQaxTriDmE9Rh%2bUo%2fbbw%2bCx0bH1g0qs3Tt7Q2sQI%3d&risl=&pid=ImgRaw&r=0" alt="Counter">
                    <p>Go to Counter</p>
                </div>
                <div class="item-button" id="chips-button" onclick="navigate('chips')" style="display:none;">
                    <img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ1g03mexKvLHz3Ra4q39GW3LRij0E6IiuHag&s" alt="Chips">
                    <p>Go to Chips</p>
                </div>
                <div class="item-button" id="drinks-button" onclick="navigate('drinks')" style="display:none;">
                    <img src="https://tiimg.tistatic.com/fp/1/008/042/1-25-liter-0-alcohol-content-coca-cola-cold-drink--488.jpg" alt="Drinks">
                    <p>Go to Drinks</p>
                </div>
            </div>
        </div>

        <!-- Receipt page -->
        <div id="receipt-page" style="display:none;">
            <h2>Receipt</h2>
            <div class="receipt">
                <table class="receipt-table">
                    <thead>
                        <tr>
                            <th>Item</th>
                            <th>Quantity</th>
                            <th>Price</th>
                        </tr>
                    </thead>
                    <tbody id="receipt-body">
                        <!-- Items will be added here -->
                    </tbody>
                </table>
                <p>Total: $<span id="total-price">0.00</span></p>
            </div>
        </div>
    </div>

    <script>
        let items = {
            chips: {name: "Chips", price: 1.5, description: "Crispy and salty."},
            drinks: {name: "Drinks", price: 1.0, description: "Cold and refreshing."}
        };
        let cart = {};

        function startProgram() {
            document.getElementById('intro').style.display = 'none';
            document.getElementById('content').style.display = 'block';
        }

        function navigate(location) {
            fetch(`/navigate?location=${location}`)
            .then(response => response.text())
            .then(data => {
                console.log(data);
                if (location === "chips" || location === "drinks") {
                    showItemPage(location);
                } else {
                    showReceiptPage();
                }
            })
            .catch(error => console.error(error));
        }

        function showItemPage(item) {
            document.getElementById('main-page').style.display = 'none';
            document.getElementById('item-page').style.display = 'block';
            document.getElementById('item-name').innerText = items[item].name;
            document.getElementById('item-description').innerText = items[item].description;
            document.getElementById('item-price').innerText = '$' + items[item].price.toFixed(2);

            if (item === "chips") {
                document.getElementById('chips-button').style.display = 'none';
                document.getElementById('drinks-button').style.display = 'block';
                document.getElementById('counter-button').setAttribute('onclick', "navigate('counterFromChips')");
            } else {
                document.getElementById('chips-button').style.display = 'block';
                document.getElementById('drinks-button').style.display = 'none';
                document.getElementById('counter-button').setAttribute('onclick', "navigate('counterFromDrinks')");
            }
            cart[item] = (cart[item] || 0) + 1;
        }

        function showReceiptPage() {
            document.getElementById('item-page').style.display = 'none';
            document.getElementById('receipt-page').style.display = 'block';
            let receiptBody = document.getElementById('receipt-body');
            receiptBody.innerHTML = '';
            let totalPrice = 0;
            for (let item in cart) {
                let row = document.createElement('tr');
                let itemName = document.createElement('td');
                let itemQuantity = document.createElement('td');
                let itemPrice = document.createElement('td');
                itemName.innerText = items[item].name;
                itemQuantity.innerText = cart[item];
                itemPrice.innerText = '$' + (items[item].price * cart[item]).toFixed(2);
                row.appendChild(itemName);
                row.appendChild(itemQuantity);
                row.appendChild(itemPrice);
                receiptBody.appendChild(row);
                totalPrice += items[item].price * cart[item];
            }
            document.getElementById('total-price').innerText = totalPrice.toFixed(2);
        }
    </script>
</body>
</html>
)=====";

// Function to navigate the robot to a location
void moveToChips();
void moveToColdDrinks();
void moveToCounterFromChips();
void moveToColdDrinksFromChips();
void moveToCounterFromColdDrinks();
void moveToChipsFromColdDrinks();
void moveForward(float distance);
void turnLeft();
void turnRight();
void navigateTo(String location) {
  if (location == "chips") {
    moveToChips();
  } else if (location == "drinks") {
    moveToColdDrinks();
  } else if (location == "counterFromChips") {
    moveToCounterFromChips();
  } else if (location == "drinksFromChips") {
    moveToColdDrinksFromChips();
  } else if (location == "counterFromDrinks") {
    moveToCounterFromColdDrinks();
  } else if (location == "chipsFromDrinks") {
    moveToChipsFromColdDrinks();
  }
}

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);

  // Set WiFi to AP mode
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start the web server
  server.on("/", []() {
    server.send_P(200, "text/html", MAIN_page);
  });

  server.on("/navigate", []() {
    String destination = server.arg("location");
    navigateTo(destination);
    server.send(200, "text/plain", "Navigating to " + destination);
  });

  server.begin();
  Serial.println("Web server started");

  // Set motor control pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Initialize the motors to be stopped
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop() {
  server.handleClient();
}

// Motor control functions as before
void moveForward(float distance) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  delay(distance * 1000); // Adjust this delay according to your speed and distance
  digitalWrite(IN1, LOW);
  digitalWrite(IN3, LOW);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  delay(500); // Adjust this delay according to your turning speed
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  delay(500); // Adjust this delay according to your turning speed
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, LOW);
}

void moveToChips() {
  // Add your specific movements to navigate to the chips location
  moveForward(2.0);
  turnRight();
  moveForward(1.0);
}

void moveToColdDrinks() {
  // Add your specific movements to navigate to the cold drinks location
  moveForward(2.0);
  turnLeft();
  moveForward(1.5);
}

void moveToCounterFromChips() {
  // Add your specific movements to navigate to the counter from the chips location
  moveForward(2.0);
  turnLeft();
  moveForward(1.0);
}

void moveToColdDrinksFromChips() {
  // Add your specific movements to navigate to the cold drinks from the chips location
  moveForward(1.0);
  turnLeft();
  moveForward(1.5);
}

void moveToCounterFromColdDrinks() {
  // Add your specific movements to navigate to the counter from the cold drinks location
  moveForward(2.5);
  turnRight();
  moveForward(1.0);
}

void moveToChipsFromColdDrinks() {
  // Add your specific movements to navigate to the chips from the cold drinks location
  moveForward(1.5);
  turnRight();
  moveForward(1.0);
}
