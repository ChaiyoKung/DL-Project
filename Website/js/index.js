let firebaseDB = firebase.database();
let bottleTransCountElement = $("#bottle-transparent-count");
let bottleColorCountElement = $("#bottle-color-count");
let bottleHistoryTableBody = $("#bottle-history-body");

function changeBottleCount(countTrans, countColor) {
  bottleTransCountElement.html(countTrans);
  bottleColorCountElement.html(countColor);
}

function appendTableHistoryRow(dateTime, isColor) {
  let tr = document.createElement("tr");
  let tdDateTime = document.createElement("td");
  tdDateTime.innerHTML = dateTime;
  let tdBottleType = document.createElement("td");
  let bottleImage = document.createElement("img");
  bottleImage.setAttribute("height", "30");
  bottleImage.classList.add("bottle-icon")
  let bottleType;
  if (isColor) {
    bottleType = "สี";
    bottleImage.setAttribute("src", "/img/plastic-bottle-color.svg");
    bottleImage.setAttribute("alt", "color plastic bottle");
  } else {
    bottleType = "ใส";
    bottleImage.setAttribute("src", "/img/plastic-bottle-trans.svg");
    bottleImage.setAttribute("alt", "transparent plastic bottle");
  }
  tdBottleType.append(bottleImage, bottleType);
  tr.append(tdDateTime, tdBottleType);
  bottleHistoryTableBody.prepend(tr);
}

function main() {
  firebaseDB.ref("records").on("value", (snapshot) => {
    let values = snapshot.val();
    let countTrans = 0;
    let countColor = 0;
    for (const key in values) {
      if (values.hasOwnProperty(key)) {
        const bottle = values[key];
        const bottleIsColor = bottle.isColor;
        const bottleDateTime = bottle.dateTime;
        if (bottleIsColor) {
          countColor++;
        } else {
          countTrans++;
        }
        appendTableHistoryRow(bottleDateTime, bottleIsColor);
      }
    }
    changeBottleCount(countTrans, countColor);
  });
}

main();
