function load() {
  const xhttp = new XMLHttpRequest();
  xhttp.onload = function () {
    document.getElementById("right-frame").textContent = this.responseText;
  };
  xhttp.open("GET", "/assets/sample.txt");
  xhttp.send();
}
