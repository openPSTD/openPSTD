<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/TestLog">
        <html>
            <body>
                <h1>Test report</h1>
                <p>
                    XSLT version: <xsl:value-of select="system-property('xsl:version')"/> <br />
                    Date: <xsl:value-of select="current-date()"/><br />
                    Time: <xsl:value-of select="current-time()"/>
                </p>
                <xsl:apply-templates/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="TestSuite">
        <h2><xsl:value-of select="@name" /></h2>
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="TestCase">
        <h4><xsl:value-of select="@name" /></h4>
        Testing time: <xsl:value-of select="TestingTime" />
        <table border="1">
            <tr>
                <th style="text-align:left">line</th>
                <th style="text-align:left">code</th>
                <th style="text-align:left">Passed</th>
            </tr>
            <xsl:apply-templates/>
        </table>
    </xsl:template>

    <xsl:template match="Info">
        <tr>
            <th><xsl:value-of select="@line"/></th>
            <th><xsl:value-of select="."/></th>
            <th bgcolor="#00FF00"></th>
        </tr>
    </xsl:template>

    <xsl:template match="Error">
        <tr>
            <th><xsl:value-of select="@line"/></th>
            <th><xsl:value-of select="."/></th>
            <th bgcolor="#FF0000"></th>
        </tr>
    </xsl:template>

</xsl:stylesheet>