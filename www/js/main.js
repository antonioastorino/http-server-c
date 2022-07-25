function changeView(ev) {
  let frames = document.getElementsByClassName("subpage");
  for (let frame of frames) {
    frame.style.display = "none";
  }
  if (ev.target.id == "button-configure") {
    document.getElementById("iframe-configure").style.display = "block";
  }
  if (ev.target.id == "button-info") {
    document.getElementById("iframe-info").style.display = "block";
  }
}

window.onload = function () {
  document.getElementById("subpage-viewer").style.height = "100vh";
  document.getElementById("subpage-viewer").style.bottom = "-30px";
};
