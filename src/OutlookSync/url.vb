Option Strict Off
Option Explicit On
Module url_helper
	
	Structure URL
		Dim Scheme As String
		Dim Host As String
		Dim Port As Integer
		Dim URI As String
		Dim Query As String
	End Structure
	
	
	' returns as type URL from a string
	Function ExtractUrl(ByVal strUrl As String) As URL
		Dim intPos1 As Short
		Dim intPos2 As Short
		
		Dim retURL As URL
		
		'1 look for a scheme it ends with ://
		intPos1 = InStr(strUrl, "://")
		
		If intPos1 > 0 Then
			retURL.Scheme = Mid(strUrl, 1, intPos1 - 1)
			strUrl = Mid(strUrl, intPos1 + 3)
		End If
		
		'2 look for a port
		intPos1 = InStr(strUrl, ":")
		intPos2 = InStr(strUrl, "/")
		
		If intPos1 > 0 And intPos1 < intPos2 Then
			' a port is specified
			retURL.Host = Mid(strUrl, 1, intPos1 - 1)
			
			If (IsNumeric(Mid(strUrl, intPos1 + 1, intPos2 - intPos1 - 1))) Then
				retURL.Port = CShort(Mid(strUrl, intPos1 + 1, intPos2 - intPos1 - 1))
			End If
		ElseIf intPos2 > 0 Then 
			retURL.Host = Mid(strUrl, 1, intPos2 - 1)
		Else
			retURL.Host = strUrl
			retURL.URI = "/"
			
			'UPGRADE_WARNING: Couldn't resolve default property of object ExtractUrl. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
			ExtractUrl = retURL
			Exit Function
		End If
		
		strUrl = Mid(strUrl, intPos2)
		
		' find a question mark ?
		intPos1 = InStr(strUrl, "?")
		
		If intPos1 > 0 Then
			retURL.URI = Mid(strUrl, 1, intPos1 - 1)
			retURL.Query = Mid(strUrl, intPos1 + 1)
		Else
			retURL.URI = strUrl
		End If
		
		'UPGRADE_WARNING: Couldn't resolve default property of object ExtractUrl. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		ExtractUrl = retURL
	End Function
	
	' url encodes a string
	'UPGRADE_NOTE: str was upgraded to str_Renamed. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1061"'
	Function URLEncode(ByVal str_Renamed As String) As String
		Dim intLen As Short
		Dim X As Short
		Dim curChar As Integer
		Dim newStr As String
		
		intLen = Len(str_Renamed)
		newStr = ""
		
		' encode anything which is not a letter or number
		For X = 1 To intLen
			curChar = Asc(Mid(str_Renamed, X, 1))
			
			
			If curChar = 32 Then
				' we can use a + sign for a space
				newStr = newStr & "+"
			ElseIf (curChar < 48 Or curChar > 57) And (curChar < 65 Or curChar > 90) And (curChar < 97 Or curChar > 122) Then 
				
				
				newStr = newStr & "%" & Hex(curChar)
			Else
				newStr = newStr & Chr(curChar)
			End If
		Next X
		
		URLEncode = newStr
	End Function
	
	' decodes a url encoded string
	'UPGRADE_NOTE: str was upgraded to str_Renamed. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1061"'
	Function UrlDecode(ByVal str_Renamed As String) As String
		Dim intLen As Short
		Dim X As Short
		Dim curChar As New VB6.FixedLengthString(1)
		Dim strCode As New VB6.FixedLengthString(2)
		
		Dim newStr As String
		
		intLen = Len(str_Renamed)
		newStr = ""
		
		For X = 1 To intLen
			curChar.Value = Mid(str_Renamed, X, 1)
			
			If curChar.Value = "%" Then
				strCode.Value = "&h" & Mid(str_Renamed, X + 1, 2)
				
				If IsNumeric(strCode.Value) Then
					curChar.Value = Chr(Int(CDbl(strCode.Value)))
				Else
					curChar.Value = ""
				End If
				X = X + 2
			End If
			
			newStr = newStr & curChar.Value
		Next X
		
		UrlDecode = newStr
	End Function
End Module