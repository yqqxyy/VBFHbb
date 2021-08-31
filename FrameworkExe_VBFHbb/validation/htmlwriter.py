# Adapted from DataQuality group

import os
from shutil import copytree

import sys
import time

## LumiBlock length (in minutes)
LBlength = 1.0

def writeHTML( name, inputDir, htmlDir, allDirsScriptDir ):
  ## browserMenu = True if you want a browser menu instead of the 
  ## allDirsScript = url of javascript to create browser menu

  # Directories will all be treated with absolute path names to avoid ambiguity
  inputDir = os.path.abspath(inputDir)
  htmlDir = os.path.abspath(htmlDir)
  
  if ( htmlDir.rfind("/")!=(len(htmlDir)-1) ):  # htmlDir needs "/" at the end
    htmlDir+="/"

  try:
    copytree(inputDir, htmlDir)
  except os.error:
    print 'Info: output directory already exists. Skipping file copying step.'

  makeAllDirsFile(htmlDir, inputDir)

  for (dirpath, dirnames, filenames) in os.walk(inputDir):
    if dirpath == inputDir:
      continue

    currentDir = dirpath.replace(inputDir, htmlDir)
    if (not os.access(currentDir, os.F_OK)):
      try:
        os.makedirs(currentDir)
      except os.error:
        print 'Cannot create directory "' + currentDir + '"; exiting.'
        sys.exit(-1)
    makeSubDirFile(name, htmlDir, currentDir, dirpath, allDirsScriptDir)
    reds = makeColorFile(name, htmlDir, currentDir, dirpath, 'Red', allDirsScriptDir)
    yellows = makeColorFile(name, htmlDir, currentDir, dirpath, 'Yellow', allDirsScriptDir)
    greens = makeColorFile(name, htmlDir, currentDir, dirpath, 'Green', allDirsScriptDir)
    makeCSSFile(htmlDir, "", currentDir.replace(htmlDir, ""))
  
  makeCSSFile( htmlDir,"", "." )

def makeAllDirsFile( htmlDir, inputDir):
  
  g=open(htmlDir+'index.html', 'w')
  g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  g.write('<head>\n')
  g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  g.write('<title>CxAODFramework Validation Results</title>\n')
  g.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  g.write('</head>\n')
  g.write('<body>\n')
  g.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font><h1>CxAODFramework Validation Results</h1>\n')  

  g.write('<tr valign="top">\n')
  g.write('<td width="500"><font class="DQGroup">\n')
  
  colorDirDict = dict()
  color, colorDirDict = getGroupColor(inputDir, colorDirDict)
  g.write('<ul>\n<li>Overall status: <font class="' + color + '">' + color + '</font>\n')

  base = inputDir.count('/')
  indent = base

  # dirpath: path to the current directory
  # dirnames: list of subdirectories in the directory
  # filenames: list of files in the directory
  for (dirpath, dirnames, filenames) in os.walk(inputDir):
    newIndent = dirpath.count('/')
    if newIndent == base:
      continue
    
    if newIndent > indent:
      g.write('<ul>\n')
      indent = newIndent
    elif newIndent < indent:
      while not newIndent == indent:
        g.write('</ul>\n')
        indent -= 1

    color, colorDirDict = getGroupColor(dirpath, colorDirDict)
    # Don't make name a link if there are no files in the directory
    if filenames:
      g.write('<li><a href="'+dirpath.replace(inputDir,'').strip('/')+'/index.html">'+os.path.basename(dirpath)+':</a> <font class="'+color+'">'+color+'</font></li>\n')      
    else:
      g.write('<li>'+os.path.basename(dirpath)+': <font class="'+color+'">'+color+'</font></li>\n')

  while not indent == base:
    g.write('<ul>\n')
    indent -=1

  g.write('</ul></td></tr></table></body></html>')
  g.close()

# Gets color classification of the directory passed in. Dictionary matches directories to colors and is used to store results.
def getGroupColor( directory, colorDirDict):
  if directory in colorDirDict:
    return colorDirDict[directory], colorDirDict

  color = 'Green'
  els = os.listdir(directory)
  for item in els:
    if os.path.isfile(os.path.join(directory, item)):
      if '_YELLOW' in item:
        color = 'Yellow'
      elif '_RED' in item:
        colorDirDict[directory] = 'Red'
        return 'Red', colorDirDict
    elif os.path.isdir(os.path.join(directory, item)):
      result, colorDirDict = getGroupColor(os.path.join(directory, item), colorDirDict)
      if result == 'Yellow':
        color = 'Yellow'
      elif result == 'Red':
        colorDirDict[directory] = 'Red'
        return 'Red', colorDirDict

  colorDirDict[directory] = color
  return color, colorDirDict
        


def makeSubDirFile(name, htmlDir, htmlSubDir, filesDir, AllDirsScriptDir):
  h=open(htmlSubDir+'/index.html','w')
  subnameTitle = htmlSubDir.replace(htmlDir,'')
  h.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  h.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  h.write('<head>\n')
  h.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  h.write('<title>'+ name + ' ' + subnameTitle + '</title>\n')
  h.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  h.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/AllDirs.js"><!-- dont contract--></script>\n')
  h.write('</head>\n')
  h.write('<body>\n')
  h.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
#  h.write('<font class="DQGroup">[<a href="')
#  for x in range(subname.count("/")):
#    h.write('../')
#  h.write('../index.html">Back</a>]</font>\n')
  h.write('<center>')
  h.write('<table>\n<tr valign="top">\n<td width="250"></td>\n')
  h.write('<td width="300" align="center"><h2>'+name+' '+subnameTitle+'</h2>\n')
  h.write('<a href="Red.html">[Only Red]</a>\n')
  h.write('<a href="Yellow.html">[Only Yellow]</a>\n')
  h.write('<a href="Green.html">[Only Green]</a></td>\n')
  h.write('<td width="250">\n<font class="Note">Click on images for details and full size.</font>\n')
  h.write('</td>\n</tr>\n</table>\n')
  h.write('<table cellpadding="4" cellspacing="20">\n')    
  files = [f for f in os.listdir(filesDir) if os.path.isfile(os.path.join(filesDir, f))]
  col = 0
  for plot in files:
    color = "Undefined"
    htmlFileName = plot
    if "_RED" in plot:
      color = "Red"
      htmlFileName = htmlFileName.replace("_RED.png", "")
    elif "_YELLOW" in plot:
      color = "Yellow"
      htmlFileName = htmlFileName.replace("_YELLOW.png", "")
    elif "_GREEN" in plot:
      color = "Green"
      htmlFileName = htmlFileName.replace("_GREEN.png", "")
    
    col += 1
    if col==1:
      h.write('<tr>\n<td class="' + color + '" align="center"><a href="'+ htmlFileName +'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'400px\')"><img src="'+ plot +'" height="200"  alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n')
    elif col==3:
      h.write('<td class="' + color + '" align="center"><a href="'+htmlFileName+'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'500px\')"><img src="'+ plot +'" height="200"  alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n</tr>\n')
      col=0
    else:
      h.write('<td class="' + color + '" align="center"><a href="'+htmlFileName+'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'400px\')"><img src="'+ plot +'" height="200" alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n')
    makeOneHistFile( htmlDir, name, subnameTitle, htmlFileName, color)
  if not (col==3):
    h.write('</tr>\n')
  h.write('</table>\n</center>\n</body>\n</html>\n')
  h.close()


def makeColorFile(name, htmlDir, htmlSubDir, filesDir, color, AllDirsScriptDir):  
  h=open(htmlSubDir+'/'+color+'.html','w')
  subnameTitle = htmlSubDir.replace(htmlDir,'')

  h.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  h.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  h.write('<head>\n')
  h.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  h.write('<title>'+ name + ' ' + subnameTitle + ' ' + color + '</title>\n')
  h.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
  h.write('<script type="text/javascript" src="'+AllDirsScriptDir+'/AllDirs.js"><!-- dont contract--></script>\n')
  h.write('</head>\n')
  h.write('<body>\n')
  h.write('<font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font>')
  h.write('<center>')
  h.write('<table>\n<tr valign="top">\n<td width="250"></td>\n')
  h.write('<td width="300"><h2>'+name+' '+subnameTitle+': ' + color + ' only</h2>\n')
  h.write('<a href="Red.html">[Only Red]</a>\n')
  h.write('<a href="Yellow.html">[Only Yellow]</a>\n')
  h.write('<a href="Green.html">[Only Green]</a></td>\n')
  h.write('<td width="250">\n<font class="Note">Click on images for details and full size.</font>\n')
  h.write('</td>\n</tr>\n</table>\n')
  h.write('<table cellpadding="4" cellspacing="20">\n')    

  files = [f for f in os.listdir(filesDir) if os.path.isfile(os.path.join(filesDir, f))]
  col = 0
  count = 0
  for plot in files:
    htmlFileName = plot
    if color == "Red" and "_RED" in plot:
      htmlFileName = htmlFileName.replace("_RED.png", "")
      count += 1
    elif color == "Yellow" and "_YELLOW" in plot:
      htmlFileName = htmlFileName.replace("_YELLOW.png", "")
      count += 1
    elif color == "Green" and "_GREEN" in plot:
      htmlFileName = htmlFileName.replace("_GREEN.png", "")
      count += 1
    else:
      continue
    
    col += 1
    if col==1:
      h.write('<tr>\n<td class="' + color + '" align="center"><a href="'+ htmlFileName +'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'400px\')"><img src="'+ plot +'" height="200"  alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n')
    elif col==3:
      h.write('<td class="' + color + '" align="center"><a href="'+htmlFileName+'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'500px\')"><img src="'+ plot +'" height="200"  alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n</tr>\n')
      col=0
    else:
      h.write('<td class="' + color + '" align="center"><a href="'+htmlFileName+'.html" class="hintanchor" onmouseover="showhint(\'' +htmlFileName+'\', this, event, \'400px\')"><img src="'+ plot +'" height="200" alt="' + name + ' ' + subnameTitle+'/'+plot+'" /></a></td>\n')
    makeOneHistFile( htmlDir, name, subnameTitle, htmlFileName, color)
  if not (col==3):
    h.write('</tr>\n')
  h.write('</table>\n')
  if count==0:
    h.write('<h3> No '+color+' histograms </h3>\n')
  h.write('</center>\n</body>\n</html>\n')
  h.close()
  return count


def makeOneHistFile( htmlDir, name, subname, plotTitle, color):  
  k=open(htmlDir+'/'+subname+'/'+plotTitle+'.html','w')
  k.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
  k.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
  k.write('<head>\n')
  k.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
  k.write('<title>'+name+ ' ' + subname+ ' ' + plotTitle+'</title>\n')
  k.write('<link rel="stylesheet" href="AutomChecks.css" type="text/css" />\n')
##   k.write('<script type=\"text/javascript\" src=\"'+javaScriptLoc +'\"><!-- dont contract-->\n</script>\n')
  k.write('</head>\n')
  k.write('<body>\n')
  k.write('<center>\n')
  k.write('<table>\n<tr valign="top">\n')
  k.write('<td width="250"><font class="DQGroup">[<a href="#" onclick="history.go(-1);return false;">Back</a>]</font></td>\n')
  k.write('<td width="500"><h2>'+name + ' ' + subname+'/'+plotTitle+'</h2></td>\n')
  k.write('<td width="250"></td>\n</tr>\n')

  k.write('</table>\n')
  k.write('<table cellpadding="10">\n<tr>\n')
  k.write('<td>\n<table width="400">\n')
  k.write('<tr><td colspan="2"><font class="Heading">Assessment Details:</font></td></tr>\n')
  k.write('<tr><td>&nbsp;</td></tr>\n')
  k.write('<tr><td align="right"><font class="DatumName">Name:</font></td>')
  k.write('<td>' +plotTitle +'</td></tr>\n' )
  k.write('<tr><td align="right"><font class="DatumName">Status:</font></td>') 
  k.write('<td><font class="'+ color +'">'+ color + '</font></td></tr>\n')

  if color == 'Red':
    plotTitle += '_RED'
  elif color == 'Yellow':
    plotTitle += '_YELLOW'
  elif color == 'Green':
    plotTitle += '_GREEN'
  k.write('</table>\n</td>\n')
  k.write('<td><a href="index.html"><img src="'+ plotTitle +'.png" alt="'+plotTitle+'.png" /></a></td>\n')
  k.write('</tr></table>\n')
  k.write('</center>\n')
  now = time.localtime()
  #lastUpdate = "Last Update: "+str(now[0])+"-"+str(now[1])+"-"+str(now[2])+" "+str(now[3])+":"+str(now[4])+" UTC"
  lastUpdate = "Last Update: "+time.strftime('%Y-%m-%d %H:%M %Z', now)
  k.write('</br><font class=\"Note\">'+lastUpdate+'</font><br />\n')
        
  k.write('</body>\n</html>\n')
  k.close()


def makeCSSFile( htmlDir, name, subname ):
  css=open(htmlDir+'/'+subname+'/'+name+'AutomChecks.css','w')
  css.write('BODY\n{\n  background: #E6E6FA;\n  color: #000000;\n  font-family: helvetica,sans-serif;\n}\n')
  css.write('H1\n{\n  font-family: helvetica,sans-serif;\n  font-size: x-large;\n  text-align: left;\n}\n')
  css.write('H2\n{\n  font-family: helvetica,sans-serif;\n  font-size: large;\n  text-align: center;\n}\n')
  css.write('H3\n{\n  font-family: helvetica,sans-serif;\n  font-size: medium;\n  text-align: left;\n}\n')
  css.write('A IMG\n{\n  border: none;\n}\n')
  css.write('FONT.Info\n{\n  color: black;\n  font-style: italic;\n}\n')
  css.write('FONT.Heading\n{\n  color: black;\n  font-weight: bold;\n  font-size: large;\n}\n')
  css.write('FONT.DatumName\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('FONT.Note\n{\n  color: black;\n  font-size: small;\n}\n')
  css.write('FONT.DQGroup\n{\n  font-size: small;\n}\n')
  css.write('FONT.Red\n{\n  color: red;\n  font-weight: bold;\n}\n')
  css.write('FONT.Yellow\n{\n  color: #ffd700;\n  font-weight: bold;\n}\n')
  css.write('FONT.Green\n{\n  color: green;\n  font-weight: bold;\n}\n')
  css.write('FONT.Disabled\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('FONT.Undefined\n{\n  color: gray;\n}\n')
  css.write('FONT.NoCheck\n{\n  color: black;\n  font-weight: bold;\n}\n')
  css.write('TD.Red\n{\n  background-color: red;\n}\n')
  css.write('TD.Yellow\n{\n  background-color: #ffd700;\n}\n')
  css.write('TD.Green\n{\n  background-color: green;\n}\n')
  css.write('TD.Disabled\n{\n background-color: black;\n}\n')
  css.write('TD.Undef\n{\n  background-color: gray;\n}\n')
  css.write('TD.NoCheck\n{\n  background-color: #E6E6FA;\n}\n')
  css.write('.hintanchor\n{\n font-weight: bold; \n color: navy; \n }\n')
  css.write('#hintbox{\n position:absolute; \n top: 0; \n background-color: lightyellow; \n  width: 150px; \n padding: 3px; \n border:1px solid black; \n')
  css.write('font:normal 15px Verdana;\n line-height:18px; \n z-index:100; \n border-right: 3px solid black; \n border-bottom: 3px solid black; \n visibility: hidden;\n }')
  css.close()

if __name__ == "__main__":
  import optparse
  parser = optparse.OptionParser(usage='usage: %prog [options] inputDirectory')
  parser.add_option('--title', default='Summary', help='Title for histograms being tested')
  parser.add_option('--outdir', default='html', help='Directory for web output')

  options, args = parser.parse_args()

  if not len(args) == 1:
    parser.print_help()
    sys.exit(1)
  inputDir = args[0]

  writeHTML(options.title, inputDir, options.outdir, 'https://atlasdqm.web.cern.ch/atlasdqm/js/')
