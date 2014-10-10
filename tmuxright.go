package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

const (
	Arrow1 = ""
	Arrow0 = ""
)

func bold(fg, bg int) string {
	return color(fg, bg, true)
}

func nobold(fg, bg int) string {
	return color(fg, bg, false)
}

func color(fg, bg int, bold bool) string {
	bgc := "default"
	if bg >= 0 {
		bgc = fmt.Sprintf("colour%d", bg)
	}
	bt := "bold"
	if !bold {
		bt = "nobold"
	}
	return fmt.Sprintf("#[fg=colour%d,bg=%s,%s,noitalics,nounderscore]", fg, bgc, bt)
}

func main() {
	hn, err := os.Hostname()
	if err != nil {
		hn = "???"
	}

	cmd := exec.Command("sh", "-c", "pmset -g batt | tail -1 | awk '{print $2}' | sed 's/\\%\\;//'")
	batt, err := cmd.CombinedOutput()
	batt = []byte(strings.TrimSpace(string(batt)))
	if err != nil {
		batt = []byte("???")
	}

	cmd2 := exec.Command("uptime")
	uptimetext, err := cmd2.CombinedOutput()
	uptimetext = []byte(strings.TrimSpace(string(uptimetext)))
	uptimes := make([]float64, 3)
	if err == nil {
		parts := strings.Split(string(uptimetext), " ")
		if len(parts) < 3 {
			uptimes[0] = 42
			uptimes[1] = 42
			uptimes[2] = 42
		} else {
			uptimes[0], _ = strconv.ParseFloat(parts[len(parts)-3], 64)
			uptimes[1], _ = strconv.ParseFloat(parts[len(parts)-2], 64)
			uptimes[2], _ = strconv.ParseFloat(parts[len(parts)-1], 64)
		}
	} else {
		uptimes[0] = 42
		uptimes[1] = 42
		uptimes[2] = 42
	}

	ms, _ := filepath.Glob(os.Getenv("HOME") + "/.mail/notify/*/new/*")
	color := nobold(237, 233)
	if len(ms) > 0 {
		color = nobold(1, 233)
	}
	mails := fmt.Sprintf("%s %d", color, len(ms))

	fmt.Printf("%s",
		nobold(233, -1)+" "+
			Arrow1+nobold(247, 233)+" "+
			string(batt)+"%"+
			nobold(241, 233)+" "+
			Arrow0+nobold(2, 233)+" "+
			fmt.Sprintf("%0.1f ", uptimes[0])+
			nobold(2, 233)+
			fmt.Sprintf("%0.1f ", uptimes[1])+
			nobold(2, 233)+
			fmt.Sprintf("%0.1f", uptimes[2])+

			nobold(241, 233)+" "+
			Arrow0+
			mails+

			nobold(236, 233)+" "+
			Arrow1+nobold(252, 236)+" "+
			time.Now().Format("2006-01-02 15:04")+
			nobold(252, 236)+" "+
			Arrow1+bold(16, 252)+" "+
			hn+
			" ")
}
