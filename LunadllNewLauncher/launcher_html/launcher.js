(function() {
	var episodeData = Launcher.getEpisodeInfo("launcher", "info.json");
	var iframeOnLoadHandler = null;
	var player1Selector = null;
	var player2Selector = null;

	function populateEpisodeTemplate(episode, doc) {
		var titleField = doc.getElementById("titleField");
		var starsField = doc.getElementById("starsField");
		var starsContainer = doc.getElementById("starsContainer");
		var creditsField = doc.getElementById("creditsField");
		var creditsContainer = doc.getElementById("creditsContainer");
		
		if (titleField) titleField.textContent = episode.title;
		if (episode.stars && starsField) {
			if (starsContainer) starsContainer.style.display = "block";
			starsField.textContent = episode.stars.toString();
			if (episode.stars == 1) {
				starsField.textContent += " Star"
			} else if (episode.stars > 1) {
				starsField.textContent += " Stars"
			}
		}
		if (episode.credits && creditsField) {
			creditsField.innerHTML = "";
			if (creditsContainer) creditsContainer.style.display = "block";
			var lines = episode.credits.trim().split("\n");
			var tableObj = doc.createElement("table");
			tableObj.className = "creditsTable";
			for (var i = 0; i < lines.length; i++) {
				var line = lines[i].trim();
				var trObj = doc.createElement("tr");
				var lineSections = line.split(":");
				if (lineSections.length == 2) {
					var tdObj = doc.createElement("th");
					tdObj.textContent = lineSections[0].trim() + ": ";
					trObj.appendChild(tdObj);
					tdObj = doc.createElement("td");
					tdObj.textContent = lineSections[1].trim();
					trObj.appendChild(tdObj);
				}
				else
				{
					var tdObj = doc.createElement("td");
					tdObj.textContent = line;
					tdObj.colSpan = "2";
					trObj.appendChild(tdObj);
				}
				tableObj.appendChild(trObj);
			}
			creditsField.appendChild(tableObj);
		}
	}
	function selectEpisodeById(i) {
		var episode = episodeData[i];
		var iframe = document.getElementById("iframer");
		
		// Remove existing iframe load listeners
		if (iframeOnLoadHandler != null)
		{
			iframe.removeEventListener("load", iframeOnLoadHandler);
		}
		
		// Set iframe SRC
		if (episode.mainPage) {
			// If the episode defines a main page, use it
			iframe.src = "../worlds/" + episode.directoryName + "/launcher/" + episode.mainPage;
		} else {
			// Otherwise... Fun stuff!
			iframeOnLoadHandler = (function() {
				var doc = iframe.contentDocument;
				populateEpisodeTemplate(episode, doc)
			});
			iframe.addEventListener("load", iframeOnLoadHandler);
			iframe.src = "default/index.html";
		}
		
		// Set player selection
		var player1Label = document.getElementById("player1Label");
		var player2Label = document.getElementById("player2Label");
		player1Label.innerHTML = "";
		player2Label.innerHTML = "";
		var populatePlayerSelector = (function(containerObj, isPlayerTwo) {
			var selectObj = document.createElement("select");
			if (isPlayerTwo) {
				// If this is the player2 dropdown, add a None option
				var optionObj = document.createElement("option");
				optionObj.value = "0";
				optionObj.textContent = "None";
				selectObj.appendChild(optionObj);
			}
			for (var idx = 0; idx < episode.allowedCharacters.length; idx++) {
				// Get character ID and character name
				var charId = episode.allowedCharacters[idx];
				var charName = episode.characterNames[charId-1];
				var optionObj = document.createElement("option");
				optionObj.value = charId.toString();
				optionObj.textContent = charName;
				selectObj.appendChild(optionObj);
			}
			containerObj.appendChild(selectObj);
			return selectObj;
		});
		if (episode.allowTwoPlayer !== false) {
			player1Label.appendChild(document.createTextNode("Player 1: "));
		} else {
			player1Label.appendChild(document.createTextNode("Character: "));
		}
		player1Selector = populatePlayerSelector(player1Label, false);
		player1Label.style.display = "inline-block";
		if (episode.allowTwoPlayer !== false) {
			player2Label.appendChild(document.createTextNode("Player 2: "));
			player2Selector = populatePlayerSelector(player2Label, true);
			player2Label.style.display = "inline-block";
		} else {
			player2Selector = null;
			player2Label.style.display = "none";
		}
		
		// Set save slot info
		var saveFileList = Launcher.getSaveInfo(episode.directoryName);
		for (var idx = 0; idx < 3; idx++) {
			var saveFile = saveFileList[idx];
			var saveSlot = document.getElementById("saveSlot").children[idx];
			var extraText = "(New Game)";
			if (saveFile.isPresent) {
				if (episode.stars > 0) {
					extraText = "(" + saveFile.starCount.toString() + "/" + episode.stars.toString() + " Stars)";
				} else {
					extraText = "(" + saveFile.starCount.toString() + " Stars)";
				}
			}
			saveSlot.textContent = (idx+1).toString() + " " + extraText;
		}
	}
	function launchSMBXIGuess() {
		var episode = episodeData[parseInt(document.getElementById("episodeSelect").value)];
		Launcher.Autostart.useAutostart = true;
		Launcher.Autostart.character1 = parseInt(player1Selector.value);
		if ((player2Selector === null) || (parseInt(player2Selector.value) == 0)) {
			Launcher.Autostart.singleplayer = true;
			Launcher.Autostart.character2 = parseInt(player1Selector.value);
		} else {
			Launcher.Autostart.singleplayer = false;
			Launcher.Autostart.character2 = parseInt(player2Selector.value);
		}
		Launcher.Autostart.saveSlot = parseInt(document.getElementById("saveSlot").value);
		Launcher.Autostart.episodeName = episode.title;
		Launcher.runSMBX();
	}
	window.onload = function() {
		"use strict";
		var selectObj, optionObj, textObj;
		selectObj = document.createElement("select");
		selectObj.id = "episodeSelect";
		for (var i = 0; i < episodeData.length; i++) {
			optionObj = document.createElement("option");
			optionObj.value = i.toString();
			optionObj.textContent = episodeData[i].title;
			selectObj.appendChild(optionObj);
		}
		selectObj.onchange = function () {
			selectEpisodeById(parseInt(this.value));
		}
		document.getElementById("episodeInfo").appendChild(selectObj);
		selectEpisodeById(parseInt(selectObj.value));
		document.getElementById("playButton").onclick = function () {
			launchSMBXIGuess();
		}
		document.getElementById("classicEditorButton").onclick = function () {
			Launcher.runSMBXEditor();
		}
		document.getElementById("newEditorButton").onclick = function () {
			alert("Not Yet Integrated");
		}
	}
})();