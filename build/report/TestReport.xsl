<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text" omit-xml-declaration="yes" indent="no"/>
<xsl:template match="/">
    <xsl:apply-templates select="TestLog"/>
</xsl:template>

<xsl:template match="TestLog">Test report
XSLT version: <xsl:value-of select="system-property('xsl:version')"/>
<xsl:apply-templates select="TestSuite">
    <xsl:with-param name="depth" select="1" />
</xsl:apply-templates>
</xsl:template>

<xsl:template match="TestSuite">
<xsl:param name="depth" /><xsl:text>
</xsl:text><xsl:call-template name="repeat">
<xsl:with-param name="output" >=</xsl:with-param>
<xsl:with-param name="count" select="$depth" />
</xsl:call-template> Suite: <xsl:value-of select="@name" />
<xsl:apply-templates select="TestSuite|TestCase" >
    <xsl:with-param name="depth" select="$depth+1" />
</xsl:apply-templates>
</xsl:template>

<xsl:template match="TestCase">
<xsl:param name="depth" /><xsl:text>
</xsl:text><xsl:call-template name="repeat">
    <xsl:with-param name="output" >-</xsl:with-param>
    <xsl:with-param name="count" select="$depth" />
</xsl:call-template><xsl:text> Case: </xsl:text><xsl:value-of select="@name" /><xsl:text>
</xsl:text><xsl:call-template name="repeat">
    <xsl:with-param name="output" >-</xsl:with-param>
    <xsl:with-param name="count" select="$depth+1" />
</xsl:call-template> Testing time: <xsl:value-of select="TestingTime" />
<xsl:apply-templates select="Info|Error" >
    <xsl:with-param name="depth" select="$depth+1" />
</xsl:apply-templates>
</xsl:template>

<xsl:template match="Info"><xsl:param name="depth" /><xsl:text>
</xsl:text><xsl:call-template name="repeat">
    <xsl:with-param name="output" >-</xsl:with-param>
    <xsl:with-param name="count" select="$depth" />
</xsl:call-template> [ ] <xsl:value-of select="@line"/>
</xsl:template>

<xsl:template match="Error"><xsl:param name="depth" /><xsl:text>
</xsl:text><xsl:call-template name="repeat">
    <xsl:with-param name="output" >-</xsl:with-param>
    <xsl:with-param name="count" select="$depth" />
</xsl:call-template> [N] <xsl:value-of select="@line"/>
</xsl:template>

<xsl:template name="repeat">
<xsl:param name="output" />
<xsl:param name="count" />
<xsl:if test="$count &gt; 0">
<xsl:value-of select="$output" />
<xsl:call-template name="repeat">
<xsl:with-param name="output" select="$output" />
<xsl:with-param name="count" select="$count - 1" />
</xsl:call-template>
</xsl:if>
</xsl:template>

</xsl:stylesheet>
