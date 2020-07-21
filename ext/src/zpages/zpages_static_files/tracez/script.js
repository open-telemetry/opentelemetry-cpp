window.onload = loadTracezTable;

const spanRow = span => "<tr><td>"
  + span.name
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 6))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 6))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td><td class='click'>"
  + String(Math.floor(Math.random() * 150))
  + "</td></tr>";


const getRow = row => spanRow(row);

const getRows = rows => rows.map(row => getRow(row)).join('');

function loadTracezTable () {
  fetch("/tracez/get/aggregations").then(res => res.json())
    .then(data => {
        document.getElementById("overview_table")
            .innerHTML = getRows(data);
    });
  document.getElementById('lastUpdate').innerHTML = new Date().toLocaleString();
}

function add () {
  fetch("/status.json").then(res => res.json())
    .then(data => {
        document.getElementById("overview_table")
            .getElementsByTagName('tbody')[0]
            .innerHTML += getRows(data);
    });
  document.getElementById('lastUpdate').innerHTML = new Date().toLocaleString();
}

function run () {
  fetch('/tracez/get/running').then(res => res.json())
      .then(d => console.log(d));
}

function buck () {
  fetch('/tracez/get/bucket/1').then(res => res.json())
      .then(d => console.log(d));
}

function err () {
  fetch('/tracez/get/error').then(res => res.json())
      .then(d => console.log(d));
}
