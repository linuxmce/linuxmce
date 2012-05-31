;; -*-emacs-lisp-*-
;;
;; Emacs startup file, e.g.  /etc/emacs/site-start.d/50lmce-screen-capture-camera.el
;; for the Debian lmce-screen-capture-camera package
;;
;; Originally contributed by Nils Naumann <naumann@unileoben.ac.at>
;; Modified by Dirk Eddelbuettel <edd@debian.org>
;; Adapted for dh-make by Jim Van Zandt <jrv@debian.org>

;; The lmce-screen-capture-camera package follows the Debian/GNU Linux 'emacsen' policy and
;; byte-compiles its elisp files for each 'emacs flavor' (emacs19,
;; xemacs19, emacs20, xemacs20...).  The compiled code is then
;; installed in a subdirectory of the respective site-lisp directory.
;; We have to add this to the load-path:
(let ((package-dir (concat "/usr/share/"
                           (symbol-name flavor)
                           "/site-lisp/lmce-screen-capture-camera")))
;; If package-dir does not exist, the lmce-screen-capture-camera package must have
;; removed but not purged, and we should skip the setup.
  (when (file-directory-p package-dir)
    (setq load-path (cons package-dir load-path))
    (autoload 'lmce-screen-capture-camera-mode "lmce-screen-capture-camera-mode"
      "Major mode for editing lmce-screen-capture-camera files." t)
    (add-to-list 'auto-mode-alist '("\\.lmce-screen-capture-camera$" . lmce-screen-capture-camera-mode))))

