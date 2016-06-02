<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="text" omit-xml-declaration="yes" indent="no"/>
    <xsl:template match="/">
        <xsl:apply-templates select="TestLog"/>
    </xsl:template>

    <xsl:template match="TestLog">
        <xsl:text>========================================================================================================================&#10;</xsl:text>
        <xsl:text>=== Test report                                                                                                      ===&#10;</xsl:text>
        <xsl:text>========================================================================================================================&#10;</xsl:text>
        <xsl:text>=== Environment ===&#10;</xsl:text>
        <xsl:text>XSLT version: </xsl:text><xsl:value-of select="system-property('xsl:version')"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Platform: </xsl:text><xsl:value-of select="BuildInfo/@platform"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Compiler: </xsl:text><xsl:value-of select="BuildInfo/@compiler"/><xsl:text>&#10;</xsl:text>
        <xsl:text>STL: </xsl:text><xsl:value-of select="BuildInfo/@stl"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Boost: </xsl:text> <xsl:value-of select="BuildInfo/@boost"/><xsl:text>&#10;</xsl:text>
        <xsl:text>=== Result summary ===&#10;</xsl:text>
        <xsl:text>Test cases: </xsl:text><xsl:value-of select="count(//TestCase)"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Checks: </xsl:text><xsl:value-of select="count(//Info)+count(//Warning)+count(//Error)"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Warnings: </xsl:text><xsl:value-of select="count(//Warning)"/><xsl:text>&#10;</xsl:text>
        <xsl:text>Errors: </xsl:text><xsl:value-of select="count(.//Error)"/><xsl:text>&#10;</xsl:text>
        <xsl:if test="count(.//Error) &gt; 0">
            <xsl:text>========================================================================================================================&#10;</xsl:text>
            <xsl:text>=== Errors                                                                                                           ===&#10;</xsl:text>
            <xsl:apply-templates select="TestSuite">
                <xsl:with-param name="depth" select="1" />
                <xsl:with-param name="OnlyError" select="1" />
            </xsl:apply-templates>
        </xsl:if>
        <xsl:if test="count(.//Error) = 0">
            <xsl:text>========================================================================================================================&#10;</xsl:text>
            <xsl:text>=== Results                                                                                                          ===&#10;</xsl:text>
            <xsl:apply-templates select="TestSuite">
                <xsl:with-param name="depth" select="1" />
                <xsl:with-param name="OnlyError" select="0" />
            </xsl:apply-templates>
        </xsl:if>
    </xsl:template>

    <xsl:template match="TestSuite">
        <xsl:param name="depth" />
        <xsl:param name="OnlyError" />
        <xsl:if test="not($OnlyError) or count(.//Error) &gt; 0">
            <xsl:call-template name="repeat">
                <xsl:with-param name="output" >+</xsl:with-param>
                <xsl:with-param name="count" select="$depth" />
            </xsl:call-template>
            <xsl:text> Suite: </xsl:text>
            <xsl:value-of select="@name" />
            <xsl:text>&#10;</xsl:text>
            <xsl:apply-templates select="TestSuite|TestCase" >
                <xsl:with-param name="depth" select="$depth+1" />
                <xsl:with-param name="OnlyError" select="$OnlyError" />
            </xsl:apply-templates>
        </xsl:if>
    </xsl:template>

    <xsl:template match="TestCase">
        <xsl:param name="depth" />
        <xsl:param name="OnlyError" />
        <xsl:if test="not($OnlyError) or count(.//Error) &gt; 0">
            <xsl:call-template name="repeat">
                <xsl:with-param name="output" >-</xsl:with-param>
                <xsl:with-param name="count" select="$depth" />
            </xsl:call-template>
            <xsl:text> Case: </xsl:text><xsl:value-of select="@name" /><xsl:text>&#10;</xsl:text>
            <xsl:call-template name="repeat">
                <xsl:with-param name="output" >-</xsl:with-param>
                <xsl:with-param name="count" select="$depth+1" />
            </xsl:call-template>
            <xsl:text> File: </xsl:text><xsl:value-of select="@file" /><xsl:text>&#10;</xsl:text>
            <xsl:call-template name="repeat">
                <xsl:with-param name="output" >-</xsl:with-param>
                <xsl:with-param name="count" select="$depth+1" />
            </xsl:call-template>
            <xsl:text> Start line: </xsl:text><xsl:value-of select="@line" /><xsl:text>&#10;</xsl:text>
            <xsl:call-template name="repeat">
                <xsl:with-param name="output" >-</xsl:with-param>
                <xsl:with-param name="count" select="$depth+1" />
            </xsl:call-template>
            <xsl:text> Testing time: </xsl:text><xsl:value-of select="TestingTime" /><xsl:text>&#10;</xsl:text>
            <xsl:if test="$OnlyError = 0">
                <xsl:apply-templates select="Info|Error|Warning" >
                    <xsl:with-param name="depth" select="$depth+1" />
                </xsl:apply-templates>
            </xsl:if>
            <xsl:if test="$OnlyError = 1">
                <xsl:apply-templates select="Error" >
                    <xsl:with-param name="depth" select="$depth+1" />
                </xsl:apply-templates>
            </xsl:if>
        </xsl:if>
    </xsl:template>

    <xsl:template match="Info">
        <xsl:param name="depth" />
        <xsl:call-template name="repeat">
            <xsl:with-param name="output" >-</xsl:with-param>
            <xsl:with-param name="count" select="$depth" />
        </xsl:call-template>
        <xsl:text> [ ] Line </xsl:text>
        <xsl:value-of select="@line"/>
        <xsl:text>&#10;</xsl:text>
    </xsl:template>

    <xsl:template match="Warning">
        <xsl:param name="depth" />
        <xsl:call-template name="repeat">
            <xsl:with-param name="output" >-</xsl:with-param>
            <xsl:with-param name="count" select="$depth" />
        </xsl:call-template>
        <xsl:text> [W] Line </xsl:text>
        <xsl:value-of select="@line"/>
        <xsl:text>&#10;</xsl:text>
    </xsl:template>

    <xsl:template match="Error">
        <xsl:param name="depth" />
        <xsl:call-template name="repeat">
            <xsl:with-param name="output" >-</xsl:with-param>
            <xsl:with-param name="count" select="$depth" />
        </xsl:call-template>
        <xsl:text> [E] Line </xsl:text>
        <xsl:value-of select="@line"/>
        <xsl:text>&#10;</xsl:text>
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
