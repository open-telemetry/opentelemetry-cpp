window.onload = loadTracezTable;

const spanRow = span => "<tr><td>"
  + span.name
  + "</td><td><a href=''>"
  + String(Math.floor(Math.random() * 6))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 6))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td><td><a href=''>"
  + String(Math.floor(Math.random() * 150))
  + "</a></td></tr>";


function loadTracezTable () {
  fetch("/status.json").then(res => res.json())
    .then(data => {
      data.forEach(span => {
        document.getElementById("records_table")
          .innerHTML += spanRow(span);
        console.log(span);
      })
    });
}
