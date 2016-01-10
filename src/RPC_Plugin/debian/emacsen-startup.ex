;; -*-emacs-lisp-*-
;;
;; Emacs startup file, e.g.  /etc/emacs/site-start.d/50lmce-rpc-plugin.el
;; for the Debian lmce-rpc-plugin package
;;
;; Originally contributed by Nils Naumann <naumann@unileoben.ac.at>
;; Modified by Dirk Eddelbuettel <edd@debian.org>
;; Adapted for dh-make by Jim Van Zandt <jrv@debian.org>

;; The lmce-rpc-plugin package follows the Debian/GNU Linux 'emacsen' policy and
;; byte-compiles its elisp files for each 'emacs flavor' (emacs19,
;; xemacs19, emacs20, xemacs20...).  The compiled code is then
;; installed in a subdirectory of the respective site-lisp directory.
;; We have to add this to the load-path:
(let ((package-dir (concat "/usr/share/"
                           (symbol-name flavor)
                           "/site-lisp/lmce-rpc-plugin")))
;; If package-dir does not exist, the lmce-rpc-plugin package must have
;; removed but not purged, and we should skip the setup.
  (when (file-directory-p package-dir)
    (setq load-path (cons package-dir load-path))
    (autoload 'lmce-rpc-plugin-mode "lmce-rpc-plugin-mode"
      "Major mode for editing lmce-rpc-plugin files." t)
    (add-to-list 'auto-mode-alist '("\\.lmce-rpc-plugin$" . lmce-rpc-plugin-mode))))

