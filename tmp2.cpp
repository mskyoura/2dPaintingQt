while (!xmlDoc.atEnd() && !xmlDoc.hasError()) {
	QXmlStreamReader::TokenType token = xmlDoc.readNext();
	if (token == QXmlStreamReader::StartElement) {

		if (xmlDoc.name() == "Record") {
		QXmlStreamAttributes attrib = xmlDoc.attributes();
		date = QDate::fromString(attrib.value("Date").toString(), "dd.MM.yyyy");
		}

		if (xmlDoc.name() == "Value") {
		valRate = german.toDouble(xmlDoc.readElementText());
		continue;
		}
	}
	if (token == QXmlStreamReader::EndElement && xmlDoc.name() == "Record")
	emit rate(date, valRate);
}